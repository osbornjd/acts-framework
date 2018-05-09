// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Particle.hpp"

std::unique_ptr<Acts::ParticleProperties>
FW::SimulatedParticle<HepMC::GenParticle>::particleProperties(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  return std::move(std::make_unique<Acts::ParticleProperties>(
      Acts::ParticleProperties({particle->momentum().x(),
                                particle->momentum().y(),
                                particle->momentum().z()},
                               particle->generated_mass(),
                               HepPID::charge(particle->pid()),
                               particle->pid(),
                               particle->id())));
}

int
FW::SimulatedParticle<HepMC::GenParticle>::id(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->id();
}

std::unique_ptr<Acts::ProcessVertex>
FW::SimulatedParticle<HepMC::GenParticle>::productionVertex(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  // Return the vertex if it exists
  if (particle->production_vertex())
    return std::move(FW::SimVertex::processVertex<HepMC::GenVertex>(
        std::make_shared<HepMC::GenVertex>(*particle->production_vertex())));
  else
    return nullptr;
}

std::unique_ptr<Acts::ProcessVertex>
FW::SimulatedParticle<HepMC::GenParticle>::endVertex(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  // Return the vertex if it exists
  if (particle->end_vertex())
    return std::move(FW::SimVertex::processVertex<HepMC::GenVertex>(
        std::make_shared<HepMC::GenVertex>(*(particle->end_vertex()))));
  else
    return nullptr;
}

int
FW::SimulatedParticle<HepMC::GenParticle>::pdgID(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->pid();
}

Acts::Vector3D
FW::SimulatedParticle<HepMC::GenParticle>::momentum(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  Acts::Vector3D mom;
  mom(0) = particle->momentum().x();
  mom(1) = particle->momentum().y();
  mom(2) = particle->momentum().z();
  return mom;
}

double
FW::SimulatedParticle<HepMC::GenParticle>::energy(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->momentum().e();
}

double
FW::SimulatedParticle<HepMC::GenParticle>::mass(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  return particle->generated_mass();
}

double
FW::SimulatedParticle<HepMC::GenParticle>::charge(
    const std::shared_ptr<HepMC::GenParticle> particle)
{
  return HepPID::charge(particle->pid());
}

void
FW::SimulatedParticle<HepMC::GenParticle>::pdgID(
    std::shared_ptr<HepMC::GenParticle> particle,
    const int                           pid)
{
  particle->set_pid(pid);
}

void
FW::SimulatedParticle<HepMC::GenParticle>::momentum(
    std::shared_ptr<HepMC::GenParticle> particle,
    const Acts::Vector3D&               mom)
{
  HepMC::FourVector fVec(mom(0), mom(1), mom(2), particle->momentum().e());
  particle->set_momentum(fVec);
}

void
FW::SimulatedParticle<HepMC::GenParticle>::energy(
    std::shared_ptr<HepMC::GenParticle> particle,
    const double                        energy)
{
  HepMC::FourVector fVec(particle->momentum().x(),
                         particle->momentum().y(),
                         particle->momentum().z(),
                         energy);
  particle->set_momentum(fVec);
}

void
FW::SimulatedParticle<HepMC::GenParticle>::mass(
    std::shared_ptr<HepMC::GenParticle> particle,
    const double                        mass)
{
  particle->set_generated_mass(mass);
}
