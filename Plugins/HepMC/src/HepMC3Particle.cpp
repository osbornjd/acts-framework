// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Particle.hpp"

#include "ACTFW/Plugins/HepMC/HepMC3Vertex.hpp"

std::unique_ptr<FW::Data::SimParticle>
FW::HepMC3Particle::particle(const std::shared_ptr<HepMC::GenParticle> particle)
{
  return std::move(std::make_unique<Data::SimParticle>(
      Data::SimParticle({0., 0., 0.},
                        {particle->momentum().x(),
                         particle->momentum().y(),
                         particle->momentum().z()},
                        particle->generated_mass(),
                        HepPID::charge(particle->pid()),
                        particle->pid(),
                        particle->id())));
}

int
FW::HepMC3Particle::id(const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->id();
}

std::unique_ptr<FW::Data::SimVertex<>>
FW::HepMC3Particle::productionVertex(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  HepMC3Vertex simVert;

  // Return the vertex if it exists
  if (particle->production_vertex())
    return std::move(simVert.processVertex(
        std::make_shared<HepMC::GenVertex>(*particle->production_vertex())));
  else
    return nullptr;
}

std::unique_ptr<FW::Data::SimVertex<>>
FW::HepMC3Particle::endVertex(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  HepMC3Vertex simVert;

  // Return the vertex if it exists
  if (particle->end_vertex())
    return std::move(simVert.processVertex(
        std::make_shared<HepMC::GenVertex>(*(particle->end_vertex()))));
  else
    return nullptr;
}

int
FW::HepMC3Particle::pdgID(const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->pid();
}

Acts::Vector3D
FW::HepMC3Particle::momentum(const std::shared_ptr<HepMC::GenParticle> particle)
{
  Acts::Vector3D mom;
  mom(0) = particle->momentum().x();
  mom(1) = particle->momentum().y();
  mom(2) = particle->momentum().z();
  return mom;
}

double
FW::HepMC3Particle::energy(const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->momentum().e();
}

double
FW::HepMC3Particle::mass(const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->generated_mass();
}

double
FW::HepMC3Particle::charge(const std::shared_ptr<HepMC::GenParticle> particle)
{
  return HepPID::charge(particle->pid());
}

void
FW::HepMC3Particle::pdgID(std::shared_ptr<HepMC::GenParticle> particle,
                          const int                           pid)
{
  particle->set_pid(pid);
}

void
FW::HepMC3Particle::momentum(std::shared_ptr<HepMC::GenParticle> particle,
                             const Acts::Vector3D&               mom)
{
  HepMC::FourVector fVec(mom(0), mom(1), mom(2), particle->momentum().e());
  particle->set_momentum(fVec);
}

void
FW::HepMC3Particle::energy(std::shared_ptr<HepMC::GenParticle> particle,
                           const double                        energy)
{
  HepMC::FourVector fVec(particle->momentum().x(),
                         particle->momentum().y(),
                         particle->momentum().z(),
                         energy);
  particle->set_momentum(fVec);
}

void
FW::HepMC3Particle::mass(std::shared_ptr<HepMC::GenParticle> particle,
                         const double                        mass)
{
  particle->set_generated_mass(mass);
}
