// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Root/MaterialEffectsWriter.hpp"
#include <TFile.h>
#include <iostream>

FW::Root::MaterialEffectsWriter::MaterialEffectsWriter(
    const FW::Root::MaterialEffectsWriter::Config& cfg,
    Acts::Logging::Level                           level)
  : Base(cfg.collection, "MaterialEffectsWriter", level)
  , m_cfg(cfg)
  , m_outputFile(nullptr)
  , m_outputTree(nullptr)
{
  // Validate the configuration
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  }

  // open the output file
  m_outputFile = new TFile(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
  if (!m_outputFile) {
    ACTS_ERROR("Could not open ROOT file'" << m_cfg.filePath << "' to write");
  }
  m_outputFile->cd();
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());
  // we can not write anything w/o a tree
  if (!m_outputTree) {
    ACTS_ERROR("No output tree available");
  }

  m_outputTree->Branch("r0", &m_r0);
  m_outputTree->Branch("r1", &m_r1);

  m_outputTree->Branch("pt0", &m_pt0);
  m_outputTree->Branch("pt1", &m_pt1);

  m_outputTree->Branch("dx", &m_dx);
  m_outputTree->Branch("dy", &m_dy);
  m_outputTree->Branch("dz", &m_dz);
  m_outputTree->Branch("dr", &m_dr);

  m_outputTree->Branch("dPx", &m_dPx);
  m_outputTree->Branch("dPy", &m_dPy);
  m_outputTree->Branch("dPz", &m_dPz);
  m_outputTree->Branch("dPt", &m_dPt);
}

FW::ProcessCode
FW::Root::MaterialEffectsWriter::endRun()
{
  m_outputFile->cd();
  m_outputTree->Write();
  m_outputFile->Close();

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::MaterialEffectsWriter::writeT(
    const AlgorithmContext& ctx,
    const std::
        vector<std::pair<std::pair<Acts::ParticleProperties, Acts::Vector3D>,
                         std::pair<Acts::ParticleProperties, Acts::Vector3D>>>&
            pProperties)
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);

  m_r0.clear();
  m_r1.clear();
  m_pt0.clear();
  m_pt1.clear();
  m_dx.clear();
  m_dy.clear();
  m_dz.clear();
  m_dr.clear();
  m_dPx.clear();
  m_dPy.clear();
  m_dPz.clear();
  m_dPt.clear();

  // loop over the collection
  for (auto& prop : pProperties) {
    m_r0.push_back(prop.first.second.perp());
    m_r1.push_back(prop.second.second.perp());

    m_pt0.push_back(prop.first.first.momentum().perp());
    m_pt1.push_back(prop.second.first.momentum().perp());

    m_dx.push_back(fabs(prop.first.second.x() - prop.second.second.x()));
    m_dy.push_back(fabs(prop.first.second.y() - prop.second.second.y()));
    m_dz.push_back(fabs(prop.first.second.z() - prop.second.second.z()));
    m_dr.push_back(fabs(prop.first.second.perp() - prop.second.second.perp()));

    m_dPx.push_back(fabs(prop.first.first.momentum().x()
                         - prop.second.first.momentum().x()));
    m_dPy.push_back(fabs(prop.first.first.momentum().y()
                         - prop.second.first.momentum().y()));
    m_dPz.push_back(fabs(prop.first.first.momentum().z()
                         - prop.second.first.momentum().z()));
    m_dPt.push_back(fabs(prop.first.first.momentum().perp()
                         - prop.second.first.momentum().perp()));
  }
  m_outputTree->Fill();

  return FW::ProcessCode::SUCCESS;
}
