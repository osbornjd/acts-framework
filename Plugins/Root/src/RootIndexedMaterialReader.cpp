// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Root/RootIndexedMaterialReader.hpp"
#include <boost/algorithm/string.hpp>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Utilities/BinUtility.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "TFile.h"
#include "TH2F.h"
#include "TIterator.h"
#include "TKey.h"
#include "TList.h"

FW::Root::RootIndexedMaterialReader::RootIndexedMaterialReader(
    const FW::Root::RootIndexedMaterialReader::Config& cfg)
  : FW::IReaderT<Acts::SurfaceMaterialMap>(), m_cfg(cfg), m_inputFile(nullptr)
{
  // Validate the configuration
  if (m_cfg.folderNameBase.empty()) {
    throw std::invalid_argument("Missing ROOT folder name");
  } else if (m_cfg.fileName.empty()) {
    throw std::invalid_argument("Missing file name");
  } else if (!m_cfg.logger) {
    throw std::invalid_argument("Missing logger");
  } else if (m_cfg.name.empty()) {
    throw std::invalid_argument("Missing service name");
  }

  // Setup ROOT I/O
  m_inputFile = TFile::Open(m_cfg.fileName.c_str());
  if (!m_inputFile) {
    throw std::ios_base::failure("Could not open '" + m_cfg.fileName);
  }
}

FW::Root::RootIndexedMaterialReader::~RootIndexedMaterialReader()
{
  m_inputFile->Close();
}

FW::ProcessCode
FW::Root::RootIndexedMaterialReader::read(
    Acts::SurfaceMaterialMap& sMaterialMap,
    size_t /*skip*/,
    const FW::AlgorithmContext* /*ctx*/)
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_read_mutex);

  // Get the list of keys from the file
  TList* tlist = m_inputFile->GetListOfKeys();
  auto   tIter = tlist->MakeIterator();
  tIter->Reset();

  // Iterate over the keys in the file
  while (TKey* key = (TKey*)(tIter->Next())) {

    // The surface material to be read in for this
    std::shared_ptr<const Acts::SurfaceMaterial> sMaterial = nullptr;

    // Remember the directory
    std::string tdName(key->GetName());
    // volume
    std::vector<std::string> splitNames;
    boost::split(splitNames, tdName, boost::is_any_of(m_cfg.voltag));
    boost::split(splitNames, splitNames[1], boost::is_any_of("_"));
    geo_id_value volID = std::stoi(splitNames[0]);
    // layer
    boost::split(splitNames, tdName, boost::is_any_of(m_cfg.laytag));
    boost::split(splitNames, splitNames[1], boost::is_any_of("_"));
    geo_id_value layID = std::stoi(splitNames[0]);
    // approach
    boost::split(splitNames, tdName, boost::is_any_of(m_cfg.apptag));
    boost::split(splitNames, splitNames[1], boost::is_any_of("_"));
    geo_id_value appID = std::stoi(splitNames[0]);
    // sensitive
    boost::split(splitNames, tdName, boost::is_any_of(m_cfg.sentag));
    // boost::split(splitNames,splitNames[1],boost::is_any_of("_"));
    geo_id_value senID = std::stoi(splitNames[0]);

    // Reconstruct the geometry ID
    Acts::GeometryID geoID(volID, Acts::GeometryID::volume_mask);
    geoID.add(layID, Acts::GeometryID::layer_mask);
    geoID.add(appID, Acts::GeometryID::approach_mask);
    geoID.add(senID, Acts::GeometryID::sensitive_mask);

    // Construct the names
    std::string bName   = tdName + "/" + m_cfg.btag;
    std::string vName   = tdName + "/" + m_cfg.vtag;
    std::string tName   = tdName + "/" + m_cfg.ttag;
    std::string x0Name  = tdName + "/" + m_cfg.x0tag;
    std::string l0Name  = tdName + "/" + m_cfg.l0tag;
    std::string aName   = tdName + "/" + m_cfg.atag;
    std::string zName   = tdName + "/" + m_cfg.ztag;
    std::string rhoName = tdName + "/" + m_cfg.rhotag;

    // Get the histograms
    TH1F* b   = dynamic_cast<TH1F*>(m_inputFile->Get(bName.c_str()));
    TH1F* v   = dynamic_cast<TH1F*>(m_inputFile->Get(vName.c_str()));
    TH2F* t   = dynamic_cast<TH2F*>(m_inputFile->Get(tName.c_str()));
    TH2F* x0  = dynamic_cast<TH2F*>(m_inputFile->Get(x0Name.c_str()));
    TH2F* l0  = dynamic_cast<TH2F*>(m_inputFile->Get(l0Name.c_str()));
    TH2F* A   = dynamic_cast<TH2F*>(m_inputFile->Get(aName.c_str()));
    TH2F* Z   = dynamic_cast<TH2F*>(m_inputFile->Get(zName.c_str()));
    TH2F* rho = dynamic_cast<TH2F*>(m_inputFile->Get(rhoName.c_str()));

    // Only go on when you have all histograms
    if (b and v and t and x0 and l0 and A and Z and rho) {
      // Get the number of bins
      int nbins0 = t->GetNbinsX();
      int nbins1 = t->GetNbinsY();

      // We need binned material properties
      if (nbins0 * nbins1 > 1) {
        // The material matrix
        Acts::MaterialPropertiesMatrix materialMatrix(
            nbins1,
            Acts::MaterialPropertiesVector(nbins1, Acts::MaterialProperties()));

        // Construct the BinUtility
        // Bin 0 is always present
        Acts::BinningValue bValue0 = (Acts::BinningValue)b->GetBinContent(1);
        float              min0    = v->GetBinContent(1);
        float              max0    = v->GetBinContent(2);
        Acts::BinUtility   bu(nbins0, min0, max0, Acts::open, bValue0);
        // Bin 1 is potentially present
        if (nbins1 > 1) {
          Acts::BinningValue bValue1 = (Acts::BinningValue)b->GetBinContent(2);
          float              min1    = v->GetBinContent(3);
          float              max1    = v->GetBinContent(4);
          bu += Acts::BinUtility(nbins1, min1, max1, Acts::open, bValue1);
        }
        // Construct the surface material
        // Get the values
        for (int ib0 = 1; ib0 <= nbins0; ++ib0) {
          for (int ib1 = 1; ib1 <= nbins1; ++ib1) {
            double dt = t->GetBinContent(ib0, ib1);
            if (dt > 0.) {
              double dx0  = x0->GetBinContent(ib0, ib1);
              double dl0  = l0->GetBinContent(ib0, ib1);
              double da   = A->GetBinContent(ib0, ib1);
              double dz   = Z->GetBinContent(ib0, ib1);
              double drho = rho->GetBinContent(ib0, ib1);
              // Create material properties
              materialMatrix[ib1][ib0]
                  = Acts::MaterialProperties(dx0, dl0, da, dz, drho, dt);
            }
          }
        }
        // Create and set the binned surface material
        sMaterial = std::make_shared<const Acts::BinnedSurfaceMaterial>(
            bu, std::move(materialMatrix));
      } else {
        // Only homogeneous material present
        double dt   = t->GetBinContent(1, 1);
        double dx0  = x0->GetBinContent(1, 1);
        double dl0  = l0->GetBinContent(1, 1);
        double da   = A->GetBinContent(1, 1);
        double dz   = Z->GetBinContent(1, 1);
        double drho = rho->GetBinContent(1, 1);
        // Create and set the homogenous surface material
        sMaterial = std::make_shared<const Acts::HomogeneousSurfaceMaterial>(
            Acts::MaterialProperties(dx0, dl0, da, dz, drho, dt));
      }
    }

    // Insert into the new collection
    sMaterialMap[geoID] = std::move(sMaterial);
  }

  // Announce success
  return FW::ProcessCode::SUCCESS;
}
