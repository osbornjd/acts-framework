// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Root/RootIndexedMaterialWriter.hpp"
#include <ios>
#include <iostream>
#include <stdexcept>
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "TFile.h"
#include "TH2F.h"

FW::Root::RootIndexedMaterialWriter::RootIndexedMaterialWriter(
    const FW::Root::RootIndexedMaterialWriter::Config& cfg)
  : FW::IWriterT<Acts::IndexedSurfaceMaterial>()
  , m_cfg(cfg)
  , m_outputFile(nullptr)
{
  // Validate the configuration
  if (m_cfg.folderNameBase.empty()) {
    throw std::invalid_argument("Missing folder name base");
  } else if (m_cfg.fileName.empty()) {
    throw std::invalid_argument("Missing file name");
  } else if (!m_cfg.logger) {
    throw std::invalid_argument("Missing logger");
  } else if (m_cfg.name.empty()) {
    throw std::invalid_argument("Missing service name");
  }

  // Setup ROOT I/O
  m_outputFile = TFile::Open(m_cfg.fileName.c_str(), "recreate");
  if (!m_outputFile) {
    throw std::ios_base::failure("Could not open '" + m_cfg.fileName);
  }
}

FW::Root::RootIndexedMaterialWriter::~RootIndexedMaterialWriter()
{
  m_outputFile->Close();
}

FW::ProcessCode
FW::Root::RootIndexedMaterialWriter::write(
    const Acts::IndexedSurfaceMaterial& ism)
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);

  // Change to the output file
  m_outputFile->cd();

  // Get the Surface material
  const Acts::SurfaceMaterial* sMaterial = ism.second.get();

  // get the geometry ID
  Acts::GeometryID geoID = ism.first;
  // decode the geometryID
  geo_id_value gvolID = geoID.value(Acts::GeometryID::volume_mask);
  geo_id_value glayID = geoID.value(Acts::GeometryID::layer_mask);
  geo_id_value gappID = geoID.value(Acts::GeometryID::approach_mask);
  geo_id_value gsenID = geoID.value(Acts::GeometryID::sensitive_mask);
  // create the directory
  std::string tdName = m_cfg.folderNameBase.c_str();
  tdName += m_cfg.voltag + std::to_string(gvolID);
  tdName += m_cfg.laytag + std::to_string(glayID);
  tdName += m_cfg.apptag + std::to_string(gappID);
  tdName += m_cfg.sentag + std::to_string(gsenID);
  // create a new directory
  m_outputFile->mkdir(tdName.c_str());
  m_outputFile->cd(tdName.c_str());

  ACTS_INFO("Writing out map at " << tdName);

  size_t bins0 = 1, bins1 = 1;
  // understand what sort of material you have in mind
  const Acts::BinnedSurfaceMaterial* bsm
      = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(sMaterial);
  if (bsm) {
    // overwrite the bin numbers
    bins0 = bsm->binUtility().bins(0);
    bins1 = bsm->binUtility().bins(1);

    // Get the binning data
    auto& binningData = bsm->binUtility().binningData();
    // 1-D or 2-D maps
    size_t binningBins = bins1 > 1 ? 2 : 1;

    // The binning type information
    TH1F* b = new TH1F(
        m_cfg.btag.c_str(), "binning", binningBins, -0.5, binningBins - 0.5);

    // The binning value information
    TH1F* v = new TH1F(m_cfg.vtag.c_str(),
                       "values",
                       2 * binningBins,
                       -0.5,
                       (2 * binningBins) - 0.5);

    b->SetBinContent(1, int(binningData[0].binvalue));
    v->SetBinContent(1, binningData[0].min);
    v->SetBinContent(2, binningData[0].max);
    if (binningBins > 1) {
      b->SetBinContent(2, int(binningData[1].binvalue));
      v->SetBinContent(3, binningData[1].min);
      v->SetBinContent(4, binningData[1].max);
    }
    b->Write();
    v->Write();
  }

  TH2F* t = new TH2F(m_cfg.ttag.c_str(),
                     "thickness [mm] ;b0 ;b1",
                     bins0,
                     -0.5,
                     bins0 - 0.5,
                     bins1,
                     -0.5,
                     bins1 - 0.5);
  TH2F* x0 = new TH2F(m_cfg.x0tag.c_str(),
                      "X_{0} [mm] ;b0 ;b1",
                      bins0,
                      -0.5,
                      bins0 - 0.5,
                      bins1,
                      -0.5,
                      bins1 - 0.5);
  TH2F* l0 = new TH2F(m_cfg.l0tag.c_str(),
                      "#Lambda_{0} [mm] ;b0 ;b1",
                      bins0,
                      -0.5,
                      bins0 - 0.5,
                      bins1,
                      -0.5,
                      bins1 - 0.5);
  TH2F* A = new TH2F(m_cfg.atag.c_str(),
                     "X_{0} [mm] ;b0 ;b1",
                     bins0,
                     -0.5,
                     bins0 - 0.5,
                     bins1,
                     -0.5,
                     bins1 - 0.5);
  TH2F* Z = new TH2F(m_cfg.ztag.c_str(),
                     "#Lambda_{0} [mm] ;b0 ;b1",
                     bins0,
                     -0.5,
                     bins0 - 0.5,
                     bins1,
                     -0.5,
                     bins1 - 0.5);
  TH2F* rho = new TH2F(m_cfg.rhotag.c_str(),
                       "#rho [g/mm^3] ;b0 ;b1",
                       bins0,
                       -0.5,
                       bins0 - 0.5,
                       bins1,
                       -0.5,
                       bins1 - 0.5);

  // loop over the material and fill
  for (size_t b0 = 0; b0 < bins0; ++b0) {
    for (size_t b1 = 0; b1 < bins1; ++b1) {
      // get the material for the bin
      auto& mat = sMaterial->materialProperties(b0, b1);
      if (mat) {
        t->SetBinContent(b0 + 1, b1 + 1, mat.thickness());
        x0->SetBinContent(b0 + 1, b1 + 1, mat.material().X0());
        l0->SetBinContent(b0 + 1, b1 + 1, mat.material().L0());
        A->SetBinContent(b0 + 1, b1 + 1, mat.material().A());
        Z->SetBinContent(b0 + 1, b1 + 1, mat.material().Z());
        rho->SetBinContent(b0 + 1, b1 + 1, mat.material().rho());
      }
    }
  }
  t->Write();
  x0->Write();
  l0->Write();
  A->Write();
  Z->Write();
  rho->Write();

  // return success
  return FW::ProcessCode::SUCCESS;
}
