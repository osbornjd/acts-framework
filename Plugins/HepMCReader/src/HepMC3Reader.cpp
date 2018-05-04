// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMCReader/HepMC3Reader.hpp"
#include <ios>
#include <iostream>
#include <stdexcept>
#include "HepMC/GenParticle.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/ReaderAsciiHepMC2.h"

FW::HepMC3Reader::HepMC3Reader(const std::string& filename, const int version)
{
  // Check if version should be 2.x or 3.x
  assert(version != 2 && version != 3);
  // Construct the reader depending on the version
  switch (version) {
  case (2):
    reader = new HepMC::ReaderAsciiHepMC2(filename);
    break;
  case (3):
    reader = new HepMC::ReaderAscii(filename);
  }
}

bool
FW::HepMC3Reader::readEvt()
{
  // Read an event
  bool success = reader->read_event(*this);
  // Store the data of the event
  if (success) storeEvent();
  // Return if the reading was successful
  return success;
}

void
FW::HepMC3Reader::readFile()
{
  // Loop through the whole file and store the data
  while (readEvt()) {
  }
}

Acts::ParticleProperties
FW::HepMC3Reader::buildParticle(
    HepMC::GenVertex::particles_in_const_iterator it)
{
  // Extract the particles information from the iterator and store it in an ACTS
  // particle object
  return std::move(Acts::ParticleProperties(
      {(*it)->momentum().x(), (*it)->momentum().y(), (*it)->momentum().z()},
      (*it)->generated_mass(),
      HepPID::charge((*it)->pdg_id()),
      (*it)->pdg_id(),
      (*it)->id()));
}

void
FW::HepMC3Reader::storeEvent()
{
  // Store the head data and the vertices
  EventStore evtStore;
  evtStore.evtHead  = initHead();
  evtStore.vertices = storeEventBody();
  // Store the event persistent
  events.push_back(std::move(evtStore));
}

FW::HepMC3Reader::EventHead
FW::HepMC3Reader::initHead()
{
  // Extract the information for the member variables of HepMC3Reader::EventHead
  // from HepMC::GenEvent and store these
  FW::HepMC3Reader::EventHead evtHead;
  evtHead.eventNumber  = this->event_number();
  evtHead.unitMomentum = (momentum_unit() == HepMC::Units::MomentumUnit::MEV
                              ? Acts::units::_MeV
                              : Acts::units::_GeV);
  evtHead.unitLength
      = (length_unit() == HepMC::Units::LengthUnit::MM ? Acts::units::_mm
                                                       : Acts::units::_cm);
  evtHead.crossSection = std::make_pair(cross_section()->cross_section,
                                        cross_section()->cross_section_error);
  if (beam_particles().first && beam_particles().second)
    evtHead.beamParticles = std::make_pair(beam_particles().first->pid(),
                                           beam_particles().second->pid());
  evtHead.weights    = &(weights());
  evtHead.posVertex  = {event_pos().x(), event_pos().y(), event_pos().z()};
  evtHead.timeVertex = event_pos().t();
  // Return the HepMC3Reader::EventHead with its data
  return std::move(evtHead);
}

std::vector<Acts::ProcessVertex>
FW::HepMC3Reader::storeEventBody()
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

const FW::HepMC3Reader::EventStore&
FW::HepMC3Reader::event(unsigned int index)
{
  return events[index];
}

const std::vector<FW::HepMC3Reader::EventStore>&
FW::HepMC3Reader::allEvents()
{
  return events;
}

const FW::HepMC3Reader::EventHead&
FW::HepMC3Reader::eventHead(unsigned int index)
{
  return events[index].evtHead;
}

const std::vector<FW::HepMC3Reader::EventHead>
FW::HepMC3Reader::allEventHeads()
{
  std::vector<FW::HepMC3Reader::EventHead> heads;
  // Loop through all events and extract the head of the event
  for (auto& event : events) heads.push_back(event.evtHead);
  return heads;
}

const Acts::ProcessVertex&
FW::HepMC3Reader::vertex(unsigned int indexEvent, unsigned int indexVertex)
{
  return events[indexEvent].vertices[indexVertex];
}

const std::vector<Acts::ProcessVertex>&
FW::HepMC3Reader::vertices(unsigned int indexEvent)
{
  return events[indexEvent].vertices;
}

const std::vector<Acts::ProcessVertex>
FW::HepMC3Reader::vertices()
{
  std::vector<Acts::ProcessVertex> vertices;
  // Loop through all events
  for (auto& event : events)
    // Loop through all vertices and store them
    for (auto& vertex : event.vertices) vertices.push_back(vertex);
  return vertices;
}
