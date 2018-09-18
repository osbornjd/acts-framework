// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Vertex.hpp"
#include "ACTFW/Plugins/HepMC/HepMC3Particle.hpp"

std::vector<FW::Data::SimParticle>
FW::SimulatedVertex<HepMC::GenVertex>::genParticlesToActs(
    const std::vector<HepMC::GenParticlePtr>& genParticles)
{
  SimulatedParticle<HepMC::GenParticle> simPart;

  std::vector<Data::SimParticle> actsParticles;
  // Translate all particles
  for (auto& genParticle : genParticles)
    actsParticles.push_back(*(
        simPart.particle(std::make_shared<HepMC::GenParticle>(*genParticle))));
  return actsParticles;
}

std::unique_ptr<FW::Data::SimVertex<>>
FW::SimulatedVertex<HepMC::GenVertex>::processVertex(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  // Create Acts vertex
  return std::move(std::make_unique<Data::SimVertex<>>(Data::SimVertex<>(
      {vertex->position().x(), vertex->position().y(), vertex->position().z()},
      genParticlesToActs(vertex->particles_in()),
      genParticlesToActs(vertex->particles_out()),
      0,  // TODO: what does process_type?
      vertex->position().t())));
}

bool
FW::SimulatedVertex<HepMC::GenVertex>::inEvent(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  return vertex->in_event();
}

int
FW::SimulatedVertex<HepMC::GenVertex>::id(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  return vertex->id();
}

std::vector<FW::Data::SimParticle>
FW::SimulatedVertex<HepMC::GenVertex>::particlesIn(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  return genParticlesToActs(vertex->particles_in());
}

std::vector<FW::Data::SimParticle>
FW::SimulatedVertex<HepMC::GenVertex>::particlesOut(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  return genParticlesToActs(vertex->particles_out());
}

Acts::Vector3D
FW::SimulatedVertex<HepMC::GenVertex>::position(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  Acts::Vector3D vec;
  vec(0) = vertex->position().x();
  vec(1) = vertex->position().y();
  vec(2) = vertex->position().z();
  return vec;
}

double
FW::SimulatedVertex<HepMC::GenVertex>::time(
    const std::shared_ptr<HepMC::GenVertex> vertex)
{
  return vertex->position().t();
}

HepMC::GenParticlePtr
FW::SimulatedVertex<HepMC::GenVertex>::actsParticleToGen(
    std::shared_ptr<Data::SimParticle> actsParticle)
{
  // Extract momentum and energy from Acts particle for HepMC::FourVector
  Acts::Vector3D mom = actsParticle->momentum();
  double         energy
      = sqrt(actsParticle->m() * actsParticle->m()
             + actsParticle->momentum().dot(actsParticle->momentum()));
  const HepMC::FourVector vec(mom(0), mom(1), mom(2), energy);
  // Create HepMC::GenParticle
  HepMC::GenParticle genParticle(vec, actsParticle->pdg());
  genParticle.set_generated_mass(actsParticle->m());

  return HepMC::SmartPointer<HepMC::GenParticle>(&genParticle);
}

void
FW::SimulatedVertex<HepMC::GenVertex>::addParticleIn(
    std::shared_ptr<HepMC::GenVertex>  vertex,
    std::shared_ptr<Data::SimParticle> particle)
{
  vertex->add_particle_in(actsParticleToGen(particle));
}

void
FW::SimulatedVertex<HepMC::GenVertex>::addParticleOut(
    std::shared_ptr<HepMC::GenVertex>  vertex,
    std::shared_ptr<Data::SimParticle> particle)
{
  vertex->add_particle_out(actsParticleToGen(particle));
}

HepMC::GenParticlePtr
FW::SimulatedVertex<HepMC::GenVertex>::matchParticles(
    const std::vector<HepMC::GenParticlePtr>& genParticles,
    std::shared_ptr<Data::SimParticle>        actsParticle)
{
  const barcode_type id = actsParticle->barcode();
  // Search HepMC::GenParticle with the same id as the Acts particle
  for (auto& genParticle : genParticles)
    if (genParticle->id() == id)
      // Return particle if found
      return genParticle;
  return nullptr;
}

void
FW::SimulatedVertex<HepMC::GenVertex>::removeParticleIn(
    std::shared_ptr<HepMC::GenVertex>  vertex,
    std::shared_ptr<Data::SimParticle> particle)
{
  // Remove particle if it exists
  if (HepMC::GenParticlePtr genParticle
      = matchParticles(vertex->particles_in(), particle))
    vertex->remove_particle_in(genParticle);
}

void
FW::SimulatedVertex<HepMC::GenVertex>::removeParticleOut(
    std::shared_ptr<HepMC::GenVertex>  vertex,
    std::shared_ptr<Data::SimParticle> particle)
{
  // Remove particle if it exists
  if (HepMC::GenParticlePtr genParticle
      = matchParticles(vertex->particles_out(), particle))
    vertex->remove_particle_out(genParticle);
}

void
FW::SimulatedVertex<HepMC::GenVertex>::position(
    const std::shared_ptr<HepMC::GenVertex> vertex,
    Acts::Vector3D                          pos)
{
  HepMC::FourVector fVec(pos(0), pos(1), pos(2), vertex->position().t());
  vertex->set_position(fVec);
}

void
FW::SimulatedVertex<HepMC::GenVertex>::time(
    const std::shared_ptr<HepMC::GenVertex> vertex,
    double                                  time)
{
  HepMC::FourVector fVec(vertex->position().x(),
                         vertex->position().y(),
                         vertex->position().z(),
                         time);
  vertex->set_position(fVec);
}
