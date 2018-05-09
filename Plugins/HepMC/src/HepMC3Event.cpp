// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"

///
/// Setter
///

void
FW::SimulatedEvent<HepMC::GenEvent>::momentumUnit(
    std::shared_ptr<HepMC::GenEvent> event,
    const double                     momentumUnit)
{
  // Check, if the momentum unit fits Acts::units::_MeV or _GeV
  HepMC::Units::MomentumUnit mom;
  if (momentumUnit == Acts::units::_MeV)
    mom = HepMC::Units::MomentumUnit::MEV;
  else if (momentumUnit == Acts::units::_GeV)
    mom = HepMC::Units::MomentumUnit::GEV;
  else {
    // Report invalid momentum unit and set GeV
    std::cout << "Invalid unit of momentum: " << momentumUnit << std::endl;
    std::cout << "Momentum unit [GeV] will be used instead" << std::endl;
    mom = HepMC::Units::MomentumUnit::GEV;
  }
  // Set units
  event->set_units(mom, event->length_unit());
}

void
FW::SimulatedEvent<HepMC::GenEvent>::lengthUnit(
    std::shared_ptr<HepMC::GenEvent> event,
    const double                     lengthUnit)
{
  // Check, if the length unit fits Acts::units::_mm or _cm
  HepMC::Units::LengthUnit len;
  if (lengthUnit == Acts::units::_mm)
    len = HepMC::Units::LengthUnit::MM;
  else if (lengthUnit == Acts::units::_cm)
    len = HepMC::Units::LengthUnit::CM;
  else {
    // Report invalid length unit and set mm
    std::cout << "Invalid unit of length: " << lengthUnit << std::endl;
    std::cout << "Length unit [mm] will be used instead" << std::endl;
    len = HepMC::Units::LengthUnit::MM;
  }

  // Set units
  event->set_units(event->momentum_unit(), len);
}

void
FW::SimulatedEvent<HepMC::GenEvent>::shiftPositionBy(
    std::shared_ptr<HepMC::GenEvent> event,
    const Acts::Vector3D&            deltaPos,
    const double                     deltaTime)
{
  // Create HepMC::FourVector from position and time for shift
  const HepMC::FourVector vec(deltaPos(0), deltaPos(1), deltaPos(2), deltaTime);
  event->shift_position_by(vec);
}

void
FW::SimulatedEvent<HepMC::GenEvent>::shiftPositionTo(
    std::shared_ptr<HepMC::GenEvent> event,
    const Acts::Vector3D&            pos,
    const double                     time)
{
  // Create HepMC::FourVector from position and time for the new position
  const HepMC::FourVector vec(pos(0), pos(1), pos(2), time);
  event->shift_position_to(vec);
}

void
FW::SimulatedEvent<HepMC::GenEvent>::shiftPositionTo(
    std::shared_ptr<HepMC::GenEvent> event,
    const Acts::Vector3D&            pos)
{
  // Create HepMC::FourVector from position and time for the new position
  const HepMC::FourVector vec(pos(0), pos(1), pos(2), event->event_pos().t());
  event->shift_position_to(vec);
}

void
FW::SimulatedEvent<HepMC::GenEvent>::shiftPositionTo(
    std::shared_ptr<HepMC::GenEvent> event,
    const double                     time)
{
  // Create HepMC::FourVector from position and time for the new position
  const HepMC::FourVector vec(event->event_pos().x(),
                              event->event_pos().y(),
                              event->event_pos().z(),
                              time);
  event->shift_position_to(vec);
}

///
/// Adder
///

HepMC::GenParticlePtr
FW::SimulatedEvent<HepMC::GenEvent>::actsParticleToGen(
    std::shared_ptr<Acts::ParticleProperties> actsParticle)
{
  // Extract momentum and energy from Acts particle for HepMC::FourVector
  Acts::Vector3D mom = actsParticle->momentum();
  double         energy
      = sqrt(actsParticle->mass() * actsParticle->mass()
             + actsParticle->momentum().dot(actsParticle->momentum()));
  const HepMC::FourVector vec(mom(0), mom(1), mom(2), energy);
  // Create HepMC::GenParticle
  HepMC::GenParticle genParticle(vec, actsParticle->pdgID());
  genParticle.set_generated_mass(actsParticle->mass());

  return HepMC::SmartPointer<HepMC::GenParticle>(&genParticle);
}

void
FW::SimulatedEvent<HepMC::GenEvent>::addParticle(
    std::shared_ptr<HepMC::GenEvent>          event,
    std::shared_ptr<Acts::ParticleProperties> particle)
{
  // Add new particle
  event->add_particle(actsParticleToGen(particle));
}

HepMC::GenVertexPtr
FW::SimulatedEvent<HepMC::GenEvent>::createGenVertex(
    const std::shared_ptr<Acts::ProcessVertex>& actsVertex)
{
  // Build HepMC::FourVector
  Acts::Vector3D          pos = actsVertex->position();
  const HepMC::FourVector vec(
      pos(0), pos(1), pos(2), actsVertex->interactionTime());

  // Create vertex
  HepMC::GenVertex genVertex(vec);

  const std::vector<Acts::ParticleProperties> particlesIn
      = actsVertex->incomingParticles();
  // Store incoming particles
  for (auto& particle : particlesIn) {
    HepMC::GenParticlePtr genParticle = actsParticleToGen(
        std::make_shared<Acts::ParticleProperties>(particle));
    genVertex.add_particle_in(genParticle);
  }
  const std::vector<Acts::ParticleProperties> particlesOut
      = actsVertex->outgoingParticles();
  // Store outgoing particles
  for (auto& particle : particlesOut) {
    HepMC::GenParticlePtr genParticle = actsParticleToGen(
        std::make_shared<Acts::ParticleProperties>(particle));
    genVertex.add_particle_out(genParticle);
  }
  return HepMC::SmartPointer<HepMC::GenVertex>(&genVertex);
}

void
FW::SimulatedEvent<HepMC::GenEvent>::addVertex(
    std::shared_ptr<HepMC::GenEvent>           event,
    const std::shared_ptr<Acts::ProcessVertex> vertex)
{
  // Add new vertex
  event->add_vertex(createGenVertex(vertex));
}

///
/// Remover
///

void
FW::SimulatedEvent<HepMC::GenEvent>::removeParticle(
    std::shared_ptr<HepMC::GenEvent>                 event,
    const std::shared_ptr<Acts::ParticleProperties>& particle)
{
  const std::vector<HepMC::GenParticlePtr> genParticles = event->particles();
  const barcode_type                       id           = particle->barcode();
  // Search HepMC::GenParticle with the same id as the Acts particle
  for (auto& genParticle : genParticles)
    if (genParticle->id() == id) {
      // Remove particle if found
      event->remove_particle(genParticle);
      break;
    }
}

bool
FW::SimulatedEvent<HepMC::GenEvent>::compareVertices(
    const std::shared_ptr<Acts::ProcessVertex>& actsVertex,
    const HepMC::GenVertexPtr&                  genVertex)
{
  // Compare position, time, number of incoming and outgoing particles between
  // both vertices. Return false if one criterium does not match, else true.
  HepMC::FourVector genVec = genVertex->position();
  if (actsVertex->interactionTime() != genVec.t()) return false;
  Acts::Vector3D actsVec = actsVertex->position();
  if (actsVec(0) != genVec.x()) return false;
  if (actsVec(1) != genVec.y()) return false;
  if (actsVec(2) != genVec.z()) return false;
  if (actsVertex->incomingParticles().size()
      != genVertex->particles_in().size())
    return false;
  if (actsVertex->outgoingParticles().size()
      != genVertex->particles_out().size())
    return false;
  return true;
}

void
FW::SimulatedEvent<HepMC::GenEvent>::removeVertex(
    std::shared_ptr<HepMC::GenEvent>            event,
    const std::shared_ptr<Acts::ProcessVertex>& vertex)
{

  const std::vector<HepMC::GenVertexPtr> genVertices = event->vertices();
  // Walk over every recorded vertex
  for (auto& genVertex : genVertices)
    if (compareVertices(vertex, genVertex)) {
      // Remove vertex if it matches actsVertex
      event->remove_vertex(genVertex);
      break;
    }
}

///
/// Getter
///

double
FW::SimulatedEvent<HepMC::GenEvent>::momentumUnit(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  // HepMC allows only MEV and GEV. This allows an easy identification.
  return (event->momentum_unit() == HepMC::Units::MomentumUnit::MEV
              ? Acts::units::_MeV
              : Acts::units::_GeV);
}

double
FW::SimulatedEvent<HepMC::GenEvent>::lengthUnit(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  // HepMC allows only MM and CM. This allows an easy identification.
  return (event->length_unit() == HepMC::Units::LengthUnit::MM
              ? Acts::units::_mm
              : Acts::units::_cm);
}

Acts::Vector3D
FW::SimulatedEvent<HepMC::GenEvent>::eventPos(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  // Extract the position from HepMC::FourVector
  Acts::Vector3D vec;
  vec(0) = event->event_pos().x();
  vec(1) = event->event_pos().y();
  vec(2) = event->event_pos().z();
  return vec;
}

double
FW::SimulatedEvent<HepMC::GenEvent>::eventTime(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  // Extract the time from HepMC::FourVector
  return event->event_pos().t();
}

std::vector<std::unique_ptr<Acts::ParticleProperties>>
FW::SimulatedEvent<HepMC::GenEvent>::particles(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  std::vector<std::unique_ptr<Acts::ParticleProperties>> actsParticles;
  const std::vector<HepMC::GenParticlePtr> genParticles = event->particles();

  // Translate all particles
  for (auto& genParticle : genParticles)
    actsParticles.push_back(
        std::move(FW::SimParticle::particleProperties<HepMC::GenParticle>(
            std::make_shared<HepMC::GenParticle>(*genParticle))));

  return std::move(actsParticles);
}

std::vector<std::unique_ptr<Acts::ProcessVertex>>
FW::SimulatedEvent<HepMC::GenEvent>::vertices(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  std::vector<std::unique_ptr<Acts::ProcessVertex>> actsVertices;
  const std::vector<HepMC::GenVertexPtr> genVertices = event->vertices();

  // Translate all vertices
  for (auto& genVertex : genVertices) {
    actsVertices.push_back(
        std::move(FW::SimVertex::processVertex<HepMC::GenVertex>(
            std::make_shared<HepMC::GenVertex>(*genVertex))));
  }
  return std::move(actsVertices);
}

std::vector<std::unique_ptr<Acts::ParticleProperties>>
FW::SimulatedEvent<HepMC::GenEvent>::beams(
    const std::shared_ptr<HepMC::GenEvent> event)
{
  std::vector<std::unique_ptr<Acts::ParticleProperties>> actsBeams;
  const std::vector<HepMC::GenParticlePtr> genBeams = event->beams();
  // Translate beam particles and store the result
  for (auto& genBeam : genBeams)
    actsBeams.push_back(
        std::move(FW::SimParticle::particleProperties<HepMC::GenParticle>(
            std::make_shared<HepMC::GenParticle>(*genBeam))));
  return std::move(actsBeams);
}
