// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_HepMC3Reader_H
#define ACTFW_PLUGINS_HepMC3Reader_H

#include <fstream>
#include <iostream>
#include <mutex>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/Reader.h"
#include "HepPID/ParticleIDMethods.hh"

namespace FW {

/// @class HepMC3Reader
///
/// An HepMC reader and converter to ACTS types.
/// The HepMC::GenEvent usually stores a single event. This class is a child
/// class from this class and extends the class by a persistent storage in ACTS
/// data types.
///
class HepMC3Reader : public HepMC::GenEvent
{
public:
  // TODO: writer for the complete file

  /// @brief This structure stores general information about an event
  struct EventHead
  {
    // Index of the event
    int eventNumber;
    // Used unit of momentum (MeV or GeV)
    double unitMomentum;
    // Used unit of length (mm or cm)
    double unitLength;
    // Storage of the cross section (first) and its uncertainty (second)
    std::pair<double, double> crossSection;
    // Beam particles. Both are 0 if not set.
    std::pair<pdg_type, pdg_type> beamParticles = std::make_pair(0, 0);
    // Used weights of the event
    HepMC::WeightContainer* weights;
    // Position of the primary vertex
    Acts::Vector3D posVertex;
    // Time of the primary vertex
    double timeVertex;
    // TODO: Vertex
  };

  /// @brief This structure stores all information about the event
  struct EventStore
  {
    // TODO: heavy ions are not recorded yet
    // Storage of general information
    EventHead evtHead;
    // Storage of every vertex and particles
    std::vector<Acts::ProcessVertex> vertices;
  };

  /// @brief Constructor
  /// @param filename path of the HepMC file that will be read
  /// @param version version of HepMC that will be used
  HepMC3Reader(const std::string& filename, const int version = 3);

  /// @brief Reader of a single event
  /// @return flag to indicate if the reading was successful
  bool
  readEvt();

  /// @brief Reader of all events in a single file
  void
  readFile();

  /// @brief Getter of a single event
  /// @param index index of the event
  /// @return structure that contains all event data
  const EventStore&
  event(unsigned int index);

  /// @brief Getter of all events
  /// @return list of all event data
  const std::vector<EventStore>&
  allEvents();

  /// @brief Getter of the head of a single event
  /// @param index index of the event
  /// @return head of the corresponding event
  const EventHead&
  eventHead(unsigned int index);

  /// @brief Getter of the head of all events
  /// @return list of heads of all events
  const std::vector<EventHead>
  allEventHeads();

  /// @brief Getter of a single vertex in an event
  /// @param indexEvent index of the event
  /// @param indexVertex index of the vertex in this event
  /// @return vertex in the event
  const Acts::ProcessVertex&
  vertex(unsigned int indexEvent, unsigned int indexVertex);

  /// @brief Getter of all vertices in an event
  /// @param indexEvent index of the event
  /// @return list of all vertices in the event
  const std::vector<Acts::ProcessVertex>&
  vertices(unsigned int indexEvent);

  /// @brief Getter of all vertices recorded
  /// @return list of all vertices recorded
  const std::vector<Acts::ProcessVertex>
  vertices();

private:
  // Storage of all events
  std::vector<EventStore> events;
  // Reader of the HepMC file
  HepMC::Reader* reader;

  /// @brief Storage of a read event
  void
  storeEvent();

  /// @brief Creates and returns the head information about an event
  /// @return head of the event
  EventHead
  initHead();

  /// @brief Creates and returns the vertices in an event
  /// @return list of vertices in an event
  std::vector<Acts::ProcessVertex>
  storeEventBody();

  /// @brief Creates and returns a particle in an event
  /// @param it iterator that points to a particle in an event
  /// @return particle in an event
  Acts::ParticleProperties
  buildParticle(HepMC::GenVertex::particles_in_const_iterator it);
};
}  // FW
#endif  // ACTFW_PLUGINS_HepMC3Reader_H
