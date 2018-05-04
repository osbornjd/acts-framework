// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMCReader/HepMCReader.hpp"
#include <ios>
#include <iostream>
#include <stdexcept>

void
FW::HepMCReader::readEvt(std::istream& is)
{
  // Read an event
  read(is);
  // Store the event
  storeEvent();
}

void
FW::HepMCReader::readFile(std::istream& is)
{
  // Loop through the whole stream
  while (!is.eof()) readEvt(is);
}

Acts::ParticleProperties
FW::HepMCReader::buildParticle(HepMC::GenVertex::particles_in_const_iterator it)
{
  // Extract the particles information from the iterator and store it in an ACTS
  // particle object
  return std::move(Acts::ParticleProperties(
      {(*it)->momentum().x(), (*it)->momentum().y(), (*it)->momentum().z()},
      (*it)->generated_mass(),
      HepPID::charge((*it)->pdg_id()),
      (*it)->pdg_id(),
      (*it)->barcode()));
}

void
FW::HepMCReader::storeEvent()
{
  // Store the head data and the vertices
  EventStore evtStore;
  evtStore.evtHead  = initHead();
  evtStore.vertices = storeEventBody();
  // Store the event persistent
  events.push_back(std::move(evtStore));
}

FW::HepMCReader::EventHead
FW::HepMCReader::initHead()
{
  // Extract the information for the member variables of HepMC3Reader::EventHead
  // from HepMC::GenEvent and store these
  FW::HepMCReader::EventHead evtHead;
  evtHead.eventNumber  = this->event_number();
  evtHead.unitMomentum = (momentum_unit() == HepMC::Units::MomentumUnit::MEV
                              ? Acts::units::_MeV
                              : Acts::units::_GeV);
  evtHead.unitLength
      = (length_unit() == HepMC::Units::LengthUnit::MM ? Acts::units::_mm
                                                       : Acts::units::_cm);
  evtHead.crossSection = std::make_pair(cross_section()->cross_section(),
                                        cross_section()->cross_section_error());
  if (beam_particles().first && beam_particles().second)
    evtHead.beamParticles = std::make_pair(beam_particles().first->barcode(),
                                           beam_particles().second->barcode());
  evtHead.weights = &(weights());
  // Return the HepMC3Reader::EventHead with its data
  return std::move(evtHead);
}

std::vector<Acts::ProcessVertex>
FW::HepMCReader::storeEventBody()
{
  std::vector<Acts::ProcessVertex> vertices;
  // Loop through every vertex in the event
  for (HepMC::GenEvent::vertex_const_iterator vtx = this->vertices_begin();
       vtx != this->vertices_end();
       ++vtx) {
    std::vector<Acts::ParticleProperties> particlesIn, particlesOut;
    // Loop through every particle that entered the vertex and store it
    for (HepMC::GenVertex::particles_in_const_iterator pIn
         = (*vtx)->particles_in_const_begin();
         pIn != (*vtx)->particles_in_const_end();
         pIn++)
      particlesIn.push_back(std::move(buildParticle(pIn)));
    // Loop through every particle that exited the vertex and store it
    for (HepMC::GenVertex::particles_out_const_iterator pOut
         = (*vtx)->particles_out_const_begin();
         pOut != (*vtx)->particles_out_const_end();
         pOut++)
      particlesOut.push_back(std::move(buildParticle(pOut)));
    // Create a vertex and store it
    Acts::ProcessVertex vertex({(*vtx)->position().x(),
                                (*vtx)->position().y(),
                                (*vtx)->position().z()},
                               (*vtx)->position().t(),
                               0,  // TODO: what does process_type?
                               particlesIn,
                               particlesOut);
    vertices.push_back(std::move(vertex));
  }
  // Return the list of all vertices in the event
  return std::move(vertices);
}

const FW::HepMCReader::EventStore&
FW::HepMCReader::event(unsigned int index)
{
  return events[index];
}

const std::vector<FW::HepMCReader::EventStore>&
FW::HepMCReader::allEvents()
{
  return events;
}

const FW::HepMCReader::EventHead&
FW::HepMCReader::eventHead(unsigned int index)
{
  return events[index].evtHead;
}

const std::vector<FW::HepMCReader::EventHead>
FW::HepMCReader::allEventHeads()
{
  std::vector<FW::HepMCReader::EventHead> heads;
  // Loop through all events and extract the head of the event
  for (auto& event : events) heads.push_back(event.evtHead);
  return heads;
}

const Acts::ProcessVertex&
FW::HepMCReader::vertex(unsigned int indexEvent, unsigned int indexVertex)
{
  return events[indexEvent].vertices[indexVertex];
}

const std::vector<Acts::ProcessVertex>&
FW::HepMCReader::vertices(unsigned int indexEvent)
{
  return events[indexEvent].vertices;
}

const std::vector<Acts::ProcessVertex>
FW::HepMCReader::vertices()
{
  std::vector<Acts::ProcessVertex> vertices;
  // Loop through all events
  for (auto& event : events)
    // Loop through all vertices and store them
    for (auto& vertex : event.vertices) vertices.push_back(vertex);
  return vertices;
}
