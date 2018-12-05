// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp"

#include <ios>
#include <iostream>
#include <stdexcept>

#include "TFile.h"

#include "Acts/Utilities/Helpers.hpp"

using Acts::VectorHelpers::eta;
using Acts::VectorHelpers::phi;
using Acts::VectorHelpers::perp;

FW::Root::RootMaterialTrackWriter::RootMaterialTrackWriter(
    const FW::Root::RootMaterialTrackWriter::Config& cfg,
    Acts::Logging::Level                             level)
  : Base(cfg.collection, "RootMaterialTrackWriter", level)
  , m_cfg(cfg)
  , m_outputFile(cfg.rootFile)
{
  // An input collection name and tree name must be specified
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  } else if (m_cfg.treeName.empty()) {
    throw std::invalid_argument("Missing tree name");
  }

  // Setup ROOT I/O
  if (m_outputFile == nullptr) {
    m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
    if (m_outputFile == nullptr) {
      throw std::ios_base::failure("Could not open '" + m_cfg.filePath);
    }
  }
  m_outputFile->cd();
  m_outputTree
      = new TTree(m_cfg.treeName.c_str(), "TTree from RootMaterialTrackWriter");
  if (m_outputTree == nullptr) throw std::bad_alloc();

  // Set the branches
  m_outputTree->Branch("v_x", &m_v_x);
  m_outputTree->Branch("v_y", &m_v_y);
  m_outputTree->Branch("v_z", &m_v_z);
  m_outputTree->Branch("v_dx", &m_v_dx);
  m_outputTree->Branch("v_dy", &m_v_dy);
  m_outputTree->Branch("v_dz", &m_v_dz);
  m_outputTree->Branch("v_phi", &m_v_phi);
  m_outputTree->Branch("v_eta", &m_v_eta);
  m_outputTree->Branch("mat_x", &m_step_x);
  m_outputTree->Branch("mat_x", &m_step_y);
  m_outputTree->Branch("mat_x", &m_step_z);
  m_outputTree->Branch("mat_step_length", &m_step_length);
  m_outputTree->Branch("mat_X0", &m_step_X0);
  m_outputTree->Branch("mat_L0", &m_step_L0);
  m_outputTree->Branch("mat_A", &m_step_A);
  m_outputTree->Branch("mat_Z", &m_step_Z);
  m_outputTree->Branch("mat_rho", &m_step_rho);
}

FW::Root::RootMaterialTrackWriter::~RootMaterialTrackWriter()
{
  m_outputFile->Close();
}

FW::ProcessCode
FW::Root::RootMaterialTrackWriter::endRun()
{
  // write the tree and close the file
  ACTS_INFO("Writing ROOT output File : " << m_cfg.filePath);
  m_outputFile->cd();
  m_outputTree->Write();
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Root::RootMaterialTrackWriter::writeT(
    const AlgorithmContext&                         ctx,
    const std::vector<Acts::RecordedMaterialTrack>& materialTracks)
{
  // Exclusive access to the tree while writing
  std::lock_guard<std::mutex> lock(m_writeMutex);

  // loop over the material tracks and write them out
  for (auto& mtrack : materialTracks) {

    // clearing the vector first
    m_step_x.clear();
    m_step_y.clear();
    m_step_z.clear();
    m_step_length.clear();
    m_step_X0.clear();
    m_step_L0.clear();
    m_step_A.clear();
    m_step_Z.clear();
    m_step_rho.clear();

    // reserve the vector then
    size_t msteps = mtrack.recordedMaterialProperties().size();
    m_step_x.reserve(msteps);
    m_step_y.reserve(msteps);
    m_step_z.reserve(msteps);
    m_step_length.reserve(msteps);
    m_step_X0.reserve(msteps);
    m_step_L0.reserve(msteps);
    m_step_A.reserve(msteps);
    m_step_Z.reserve(msteps);
    m_step_rho.reserve(msteps);

    // reset the global counter
    m_tX0 = 0.;
    m_tL0 = 0.;

    // set the track information at vertex
    m_v_x   = mtrack.position().x();
    m_v_y   = mtrack.position().y();
    m_v_z   = mtrack.position().z();
    m_v_dx  = mtrack.direction().x();
    m_v_dy  = mtrack.direction().y();
    m_v_dz  = mtrack.direction().z();
    m_v_phi = phi(mtrack.direction());
    m_v_eta = eta(mtrack.direction());

    // an now loop over the material
    for (auto& mstep : mtrack.recordedMaterialProperties()) {
      // the material step position information
      m_step_x.push_back(mstep.second.x());
      m_step_y.push_back(mstep.second.y());
      m_step_z.push_back(mstep.second.z());
      // the material information
      auto mprops = mstep.first;
      m_step_length.push_back(mprops.thickness());
      m_step_X0.push_back(mprops.averageX0());
      m_step_L0.push_back(mprops.averageL0());
      m_step_A.push_back(mprops.averageA());
      m_step_Z.push_back(mprops.averageZ());
      m_step_rho.push_back(mprops.averageRho());
    }

    // write to
    m_outputTree->Fill();
  }

  // return success
  return FW::ProcessCode::SUCCESS;
}
