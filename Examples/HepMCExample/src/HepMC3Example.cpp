// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"
#include "ACTFW/Plugins/HepMC/HepMC3Reader.hpp"
#include "HepMC/ReaderAscii.h"
#include "HepPID/ParticleName.hh"

///
/// Straight forward example of reading a HepMC3 file.
///
int
main(int argc, char* argv[])
{
  std::cout << "Preparing reader " << std::flush;
  HepMC::ReaderAscii reader("test.hepmc3");
  if (FW::SimReader::status<HepMC::ReaderAscii, HepMC::GenEvent>(reader))
    std::cout << "succesful" << std::endl;
  else
    std::cout << "failed" << std::endl;

  std::shared_ptr<HepMC::GenEvent> genevt(new HepMC::GenEvent());

  std::cout << "Reading event " << std::flush;
  if (FW::SimReader::readEvent<HepMC::ReaderAscii, HepMC::GenEvent>(reader,
                                                                    genevt))
    std::cout << "succesful" << std::endl;
  else
    std::cout << "failed" << std::endl;

  std::cout << std::endl;
  std::cout << "Event data:" << std::endl;
  std::cout << "Units: ";
  if (FW::SimEvent::momentumUnit<HepMC::GenEvent>(genevt) == Acts::units::_GeV)
    std::cout << "[GEV], ";
  else if (FW::SimEvent::momentumUnit<HepMC::GenEvent>(genevt)
           == Acts::units::_MeV)
    std::cout << "[MeV], ";
  if (FW::SimEvent::lengthUnit<HepMC::GenEvent>(genevt) == Acts::units::_mm)
    std::cout << "[mm]" << std::endl;
  else if (FW::SimEvent::lengthUnit<HepMC::GenEvent>(genevt)
           == Acts::units::_cm)
    std::cout << "[cm]" << std::endl;
  Acts::Vector3D evtPos = FW::SimEvent::eventPos<HepMC::GenEvent>(genevt);
  std::cout << "Event position: " << evtPos(0) << ", " << evtPos(1) << ", "
            << evtPos(2) << std::endl;
  std::cout << "Event time: "
            << FW::SimEvent::eventTime<HepMC::GenEvent>(genevt) << std::endl;

  std::cout << "Beam particles: ";
  std::vector<std::unique_ptr<Acts::ParticleProperties>> beam
      = FW::SimEvent::beams<HepMC::GenEvent>(genevt);
  if (beam.empty())
    std::cout << "none" << std::endl;
  else {
    for (auto& pbeam : beam)
      std::cout << HepPID::particleName(pbeam->pdgID()) << " ";
    std::cout << std::endl;
  }

  std::cout << std::endl << "Vertices: ";
  std::vector<std::unique_ptr<Acts::ProcessVertex>> vertices
      = FW::SimEvent::vertices<HepMC::GenEvent>(genevt);
  if (vertices.empty())
    std::cout << "none" << std::endl;
  else {
    std::cout << std::endl;
    for (auto& vertex : vertices) {
      std::vector<Acts::ParticleProperties> particlesIn
          = vertex->incomingParticles();
      for (auto& particle : particlesIn)
        std::cout << HepPID::particleName(particle.pdgID()) << " ";
      std::cout << "-> ";
      std::vector<Acts::ParticleProperties> particlesOut
          = vertex->outgoingParticles();
      for (auto& particle : particlesOut)
        std::cout << HepPID::particleName(particle.pdgID()) << " ";
      std::cout << "\t@(" << vertex->interactionTime() << ", "
                << vertex->position()(0) << ", " << vertex->position()(1)
                << ", " << vertex->position()(2) << ")" << std::endl;
    }
    std::cout << std::endl;
  }

  std::cout << "Total particle record:" << std::endl;
  std::vector<std::unique_ptr<Acts::ParticleProperties>> particles
      = FW::SimEvent::particles<HepMC::GenEvent>(genevt);
  for (auto& particle : particles)
    std::cout << HepPID::particleName(particle->pdgID())
              << "\tID:" << particle->barcode() << ", momentum: ("
              << particle->momentum()(0) << ", " << particle->momentum()(1)
              << ", " << particle->momentum()(2)
              << "), mass:  " << particle->mass() << std::endl;

  std::cout << std::endl << "Initial to final state: ";
  std::vector<std::unique_ptr<Acts::ParticleProperties>> fState
      = FW::SimEvent::finalState<HepMC::GenEvent>(genevt);
  for (auto& pbeam : beam)
    std::cout << HepPID::particleName(pbeam->pdgID()) << " ";
  std::cout << "-> ";
  for (auto& fs : fState) std::cout << HepPID::particleName(fs->pdgID()) << " ";
  std::cout << std::endl;
}
