// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Vertex.hpp"
#include "ACTFW/Plugins/HepMC/HepMC3Particle.hpp"

std::unique_ptr<Acts::ProcessVertex>
FW::SimulatedVertex<HepMC::GenVertex>::processVertex(
    const HepMC::GenVertex* vertex)
{
  const std::vector<HepMC::GenParticlePtr> genParticlesIn
      = vertex->particles_in();
  std::vector<Acts::ParticleProperties> actsParticlesIn;
  // Translate all incoming particles
  for (auto& genParticle : genParticlesIn)
    actsParticlesIn.push_back(
        *(FW::SimParticle::particleProperties<HepMC::GenParticle>(
            &*genParticle)));

  const std::vector<HepMC::GenParticlePtr> genParticlesOut
      = vertex->particles_out();
  std::vector<Acts::ParticleProperties> actsParticlesOut;
  // Translate all outgoing particles
  for (auto& genParticle : genParticlesOut)
    actsParticlesOut.push_back(
        *(FW::SimParticle::particleProperties<HepMC::GenParticle>(
            &*genParticle)));

  // Create Acts vertex
  return std::make_unique<Acts::ProcessVertex>(Acts::ProcessVertex(
      {vertex->position().x(), vertex->position().y(), vertex->position().z()},
      vertex->position().t(),
      0,  // TODO: what does process_type?
      actsParticlesIn,
      actsParticlesOut));
}

bool
FW::SimulatedVertex<HepMC::GenVertex>::inEvent(const HepMC::GenVertex* vertex)
{
  return false;
}

int
FW::SimulatedVertex<HepMC::GenVertex>::id(const HepMC::GenVertex* vertex)
{
  return 0;
}

void
FW::SimulatedVertex<HepMC::GenVertex>::addParticleIn(
    HepMC::GenVertex*         vertex,
    Acts::ParticleProperties* particle)
{
}

void
FW::SimulatedVertex<HepMC::GenVertex>::addParticleOut(
    HepMC::GenVertex*         vertex,
    Acts::ParticleProperties* particle)
{
}

void
FW::SimulatedVertex<HepMC::GenVertex>::removeParticleIn(
    HepMC::GenVertex*         vertex,
    Acts::ParticleProperties* particle)
{
}

void
FW::SimulatedVertex<HepMC::GenVertex>::removeParticleOut(
    HepMC::GenVertex*         vertex,
    Acts::ParticleProperties* particle)
{
}

std::vector<Acts::ParticleProperties>
FW::SimulatedVertex<HepMC::GenVertex>::particlesIn(
    const HepMC::GenVertex* vertex)
{
  std::vector<Acts::ParticleProperties> asdf;
  return asdf;
}

std::vector<Acts::ParticleProperties>
FW::SimulatedVertex<HepMC::GenVertex>::particlesOut(
    const HepMC::GenVertex* vertex)
{
  std::vector<Acts::ParticleProperties> asdf;
  return asdf;
}

Acts::Vector3D
FW::SimulatedVertex<HepMC::GenVertex>::position(const HepMC::GenVertex* vertex)
{
  Acts::Vector3D vec;
  return vec;
}

double
FW::SimulatedVertex<HepMC::GenVertex>::time(const HepMC::GenVertex* vertex)
{
  return 0;
}
