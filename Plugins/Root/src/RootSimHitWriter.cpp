// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Root/RootSimHitWriter.hpp"
#include <ios>
#include <stdexcept>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Plugins/Digitization/DigitizationModule.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"
#include "Acts/Plugins/Digitization/Segmentation.hpp"

FW::Root::RootSimHitWriter::RootSimHitWriter(
    const FW::Root::RootSimHitWriter::Config& cfg,
    Acts::Logging::Level                         level)
  : Base(cfg.collection, "RootSimHitWriter", level), m_cfg(cfg)
{
  // An input collection name and tree name must be specified
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  } else if (m_cfg.treeName.empty()) {
    throw std::invalid_argument("Missing tree name");
  }

  // Setup ROOT I/O
  m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
  if (!m_outputFile) {
    throw std::ios_base::failure("Could not open '" + m_cfg.filePath);
  }
}

FW::Root::RootSimHitWriter::~RootSimHitWriter()
{
  m_outputFile->Close();
}

FW::ProcessCode
FW::Root::RootSimHitWriter::endRun()
{
  ACTS_INFO("Wrote particles to tree '" << m_cfg.treeName << "' in '"
                                        << m_cfg.filePath
                                        << "'");
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootSimHitWriter::writeT(
    const AlgorithmContext& ctx,
    const FW::DetectorData<geo_id_value, Data::SimHit<Data::Particle> >& fhits)
{
  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_writeMutex);

  // we get the event number
  int eventNr = ctx.eventNumber;

  m_outputFile->cd();

  std::string treeName = m_cfg.treeName.c_str();
  treeName += std::to_string(eventNr);

  TTree* outputTree
      = new TTree(treeName.c_str(), "TTree from RootSimHitWriter");
  if (!outputTree) throw std::bad_alloc();

  // Set the branches
  outputTree->Branch("volumeID", &m_volumeID);
  outputTree->Branch("layerID", &m_layerID);
  outputTree->Branch("surfaceID", &m_surfaceID);
  outputTree->Branch("g_x", &m_x);
  outputTree->Branch("g_y", &m_y);
  outputTree->Branch("g_z", &m_z);
  outputTree->Branch("d_x", &m_dx);
  outputTree->Branch("d_y", &m_dy);
  outputTree->Branch("d_z", &m_dz);
  outputTree->Branch("value", &m_value);

  // loop over the planar fatras hits in this event
  for (auto& volumeData : fhits) {
    for (auto& layerData : volumeData.second) {
      for (auto& moduleData : layerData.second) {
        for (auto& hit : moduleData.second) {
          // volume / layer and surface identification
          m_volumeID  = volumeData.first;
          m_layerID   = layerData.first;
          m_surfaceID = moduleData.first;
          m_x         = hit.position.x();
          m_y         = hit.position.y();
          m_z         = hit.position.z();
          m_dx        = hit.direction.x();
          m_dy        = hit.direction.y();
          m_dz        = hit.direction.z();
          m_value     = hit.value;
          // fill the tree
          outputTree->Fill();
        }
      }
    }
  }
  outputTree->Write();
  return FW::ProcessCode::SUCCESS;
}
