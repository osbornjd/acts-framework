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
    const HepMC::GenParticle* particle)
{
  return std::make_unique<Acts::ParticleProperties>(
      Acts::ParticleProperties({particle->momentum().x(),
                                particle->momentum().y(),
                                particle->momentum().z()},
                               particle->generated_mass(),
                               HepPID::charge(particle->pid()),
                               particle->pid(),
                               particle->id()));
}

int
FW::SimulatedParticle<HepMC::GenParticle>::id(
    const HepMC::GenParticle* particle)
{
  return particle->id();
}

std::unique_ptr<Acts::ProcessVertex>
FW::SimulatedParticle<HepMC::GenParticle>::productionVertex(
    const HepMC::GenParticle* particle)
{
  // Return the vertex if it exists
  if (particle->production_vertex())
    return std::move(SimVertex::processVertex<HepMC::GenVertex>(
        &*(particle->production_vertex())));
  else
    return nullptr;
}

std::unique_ptr<Acts::ProcessVertex>
FW::SimulatedParticle<HepMC::GenParticle>::endVertex(
    const HepMC::GenParticle* particle)
{
  // Return the vertex if it exists
  if (particle->end_vertex())
    return std::move(
        SimVertex::processVertex<HepMC::GenVertex>(&*(particle->end_vertex())));
  else
    return nullptr;
}

int
FW::SimulatedParticle<HepMC::GenParticle>::pdgID(
    const HepMC::GenParticle* particle)
{
  return particle->pid();
}

Acts::Vector3D
FW::SimulatedParticle<HepMC::GenParticle>::momentum(
    const HepMC::GenParticle* particle)
{
  Acts::Vector3D mom;
  mom(0) = particle->momentum().x();
  mom(1) = particle->momentum().y();
  mom(2) = particle->momentum().z();
  return mom;
}

double
FW::SimulatedParticle<HepMC::GenParticle>::energy(
    const HepMC::GenParticle* particle)
{
  return particle->momentum().e();
}

double
FW::SimulatedParticle<HepMC::GenParticle>::mass(
    const HepMC::GenParticle* particle)
{
  return particle->generated_mass();
}

double
FW::SimulatedParticle<HepMC::GenParticle>::charge(
    const HepMC::GenParticle* particle)
{
  return HepPID::charge(particle->pid());
}

void
FW::SimulatedParticle<HepMC::GenParticle>::pdgID(HepMC::GenParticle* particle,
                                                 const int           pid)
{
  particle->set_pid(pid);
}

void
FW::SimulatedParticle<HepMC::GenParticle>::momentum(
    HepMC::GenParticle*   particle,
    const Acts::Vector3D& mom)
{
  HepMC::FourVector fVec(mom(0), mom(1), mom(2), particle->momentum().e());
  particle->set_momentum(fVec);
}

void
FW::SimulatedParticle<HepMC::GenParticle>::energy(HepMC::GenParticle* particle,
                                                  const double        energy)
{
  HepMC::FourVector fVec(particle->momentum().x(),
                         particle->momentum().y(),
                         particle->momentum().z(),
                         energy);
  particle->set_momentum(fVec);
}

void
FW::SimulatedParticle<HepMC::GenParticle>::mass(HepMC::GenParticle* particle,
                                                const double        mass)
{
  particle->set_generated_mass(mass);
}
