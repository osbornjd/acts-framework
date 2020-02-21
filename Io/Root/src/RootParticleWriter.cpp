// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Io/Root/RootParticleWriter.hpp"

#include <ios>
#include <stdexcept>

#include <TFile.h>
#include <TTree.h>

#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

FW::RootParticleWriter::RootParticleWriter(
    const FW::RootParticleWriter::Config& cfg,
    Acts::Logging::Level                  lvl)
  : WriterT(cfg.inputParticles, "RootParticleWriter", lvl), m_cfg(cfg)
{
  // inputParticles is already checked by base constructor
  if (m_cfg.filePath.empty()) {
    throw std::invalid_argument("Missing file path");
  }
  if (m_cfg.treeName.empty()) {
    throw std::invalid_argument("Missing tree name");
  }

  // open root file and create the tree
  m_outputFile = TFile::Open(m_cfg.filePath.c_str(), m_cfg.fileMode.c_str());
  if (m_outputFile == nullptr) {
    throw std::ios_base::failure("Could not open '" + m_cfg.filePath + "'");
  }
  m_outputFile->cd();
  m_outputTree = new TTree(m_cfg.treeName.c_str(), m_cfg.treeName.c_str());
  if (m_outputTree == nullptr) { throw std::bad_alloc(); }

  // setup the branches
  m_outputTree->Branch("event_id", &m_eventId);
  m_outputTree->Branch("particle_id", &m_particleId, "particle_id/l");
  m_outputTree->Branch("particle_type", &m_particleType);
  m_outputTree->Branch("process", &m_process);
  m_outputTree->Branch("vx", &m_vx);
  m_outputTree->Branch("vy", &m_vy);
  m_outputTree->Branch("vz", &m_vz);
  m_outputTree->Branch("vt", &m_vt);
  m_outputTree->Branch("px", &m_px);
  m_outputTree->Branch("py", &m_py);
  m_outputTree->Branch("pz", &m_pz);
  m_outputTree->Branch("m", &m_m);
  m_outputTree->Branch("q", &m_q);
  m_outputTree->Branch("eta", &m_eta);
  m_outputTree->Branch("phi", &m_phi);
  m_outputTree->Branch("pt", &m_pt);
  m_outputTree->Branch("vertex_primary", &m_vertexPrimary);
  m_outputTree->Branch("vertex_secondary", &m_vertexSecondary);
  m_outputTree->Branch("particle", &m_particle);
  m_outputTree->Branch("parent_particle", &m_parentParticle);
}

FW::RootParticleWriter::~RootParticleWriter()
{
  if (m_outputFile) { m_outputFile->Close(); }
}

FW::ProcessCode
FW::RootParticleWriter::endRun()
{
  if (m_outputFile) {
    m_outputFile->cd();
    m_outputTree->Write();
    ACTS_INFO("Wrote particles to tree '" << m_cfg.treeName << "' in '"
                                          << m_cfg.filePath << "'");
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::RootParticleWriter::writeT(const AlgorithmContext& ctx,
                               const SimParticles&     particles)
{
  if (not m_outputFile) {
    ACTS_ERROR("Missing output file");
    return ProcessCode::ABORT;
  }

  // ensure exclusive access to tree/file while writing
  std::lock_guard<std::mutex> lock(m_writeMutex);

  m_eventId = ctx.eventNumber;
  for (const auto& particle : particles) {
    m_particleId   = particle.barcode().value();
    m_particleType = particle.pdg();
    m_process      = particle.barcode().process();
    // collect the information
    m_vx = particle.position().x() / Acts::UnitConstants::mm;
    m_vy = particle.position().y() / Acts::UnitConstants::mm;
    m_vz = particle.position().z() / Acts::UnitConstants::mm;
    m_vt = particle.time() / Acts::UnitConstants::ns;
    m_px = particle.momentum().x() / Acts::UnitConstants::GeV;
    m_py = particle.momentum().y() / Acts::UnitConstants::GeV;
    m_pz = particle.momentum().z() / Acts::UnitConstants::GeV;
    m_m  = particle.m() / Acts::UnitConstants::GeV;
    m_q  = particle.q() / Acts::UnitConstants::e;
    // derived kinematic quantities
    m_eta = Acts::VectorHelpers::eta(particle.momentum());
    m_phi = Acts::VectorHelpers::phi(particle.momentum());
    m_pt  = Acts::VectorHelpers::perp(particle.momentum())
        / Acts::UnitConstants::GeV;
    // decoded barcode components
    m_vertexPrimary   = particle.barcode().vertexPrimary();
    m_vertexSecondary = particle.barcode().vertexSecondary();
    m_particle        = particle.barcode().particle();
    m_parentParticle  = particle.barcode().parentParticle();
    m_outputTree->Fill();
  }

  return ProcessCode::SUCCESS;
}
