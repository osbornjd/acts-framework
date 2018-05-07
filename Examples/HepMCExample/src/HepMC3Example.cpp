// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <fstream>
#include <memory>
#include "ACTFW/Plugins/HepMC/HepMC3Run.hpp"
#include "HepMC/Print.h"
#include "HepMC/ReaderAscii.h"
#include "HepPID/ParticleName.hh"

///
/// Straight forward example of reading a HepMC3 file.
///
int
main(int argc, char* argv[])
{
  FW::HepMC3Run hepmc3run("example.hepmc3");
  hepmc3run.readEvent();

  const std::vector<std::shared_ptr<Acts::ProcessVertex>> vertices2
      = hepmc3run.event(0)->vertices();
  std::cout << "Number of vertices: " << vertices2.size() << std::endl;
  const std::vector<Acts::ParticleProperties> incoming2
      = vertices2[0]->incomingParticles();
  std::cout << "Incoming particles: " << incoming2.size() << std::endl;
  const std::vector<Acts::ParticleProperties> outgoing2
      = vertices2[0]->outgoingParticles();
  std::cout << "Outgoing particles: " << outgoing2.size() << std::endl;
  std::cout << "First vertex: ";
  for (Acts::ParticleProperties in : incoming2)
    std::cout << HepPID::particleName(in.pdgID()) << " ";
  std::cout << " -> ";
  for (Acts::ParticleProperties out : outgoing2)
    std::cout << HepPID::particleName(out.pdgID()) << " ";
  std::cout << std::endl;
  std::cout << "Beams: ";
  std::vector<std::shared_ptr<Acts::ParticleProperties>> beams
      = hepmc3run.event(0)->beams();
  for (auto& beam : beams)
    std::cout << HepPID::particleName(beam->pdgID()) << " ";
  std::cout << std::endl;
}
