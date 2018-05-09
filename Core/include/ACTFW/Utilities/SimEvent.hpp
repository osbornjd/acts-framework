// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include "ACTS/EventData/ParticleDefinitions.hpp"

namespace FW {

//~ /// @struct SimulatedVertex
//~ ///
//~ /// This structure is the default structure for vertices from external
//~ /// datatypes. It allows calling the functions that always handle primitive
//or
//~ /// Acts data types.
//~ ///
template <class E>
struct SimulatedEvent
{
};

namespace SimEvent {

  ///
  /// Setter
  ///
  
  //~ /// @brief Sets new units for momentums and lengths.
  //~ /// @note The allowed units are MeV and Gev or mm and cm
  //~ /// @param newMomentumUnit new unit of momentum
  //~ /// @param newLengthUnit new unit of length
  template <class E>
  static void
  momentumUnit(std::shared_ptr<E> event, const double momentumUnit)
  {
    SimulatedEvent<E>::setMomentumUnit(event, momentumUnit);
  }

  template <class E>
  static void
  lengthUnit(std::shared_ptr<E> event, const double lengthUnit)
  {
    SimulatedEvent<E>::setLengthUnit(event, lengthUnit);
  }

  //~ /// @brief Shifts the positioning of an event in space and time
  //~ /// @param deltaPos relative spatial shift that will be applied
  //~ /// @param deltaTime relative time shift that will be applied
  template <class E>
  static void
  shiftPositionBy(std::shared_ptr<E>                    event,
                  const Acts::Vector3D& deltaPos,
                  const double          deltaTime)
  {
    SimulatedEvent<E>::shiftPositionBy(event, deltaPos, deltaTime);
  }

  //~ /// @brief Shifts the positioning of an event in space and time
  //~ /// @param deltaPos relative spatial shift that will be applied
  //~ /// @param deltaTime relative time shift that will be applied
  template <class E>
  static void
  shiftPositionTo(std::shared_ptr<E>                    event,
                  const Acts::Vector3D& deltaPos,
                  const double          deltaTime)
  {
    SimulatedEvent<E>::shiftPositionTo(event, deltaPos, deltaTime);
  }

  template <class E>
  static void
  shiftPositionTo(std::shared_ptr<E> event, const Acts::Vector3D& deltaPos)
  {
    SimulatedEvent<E>::shiftPositionTo(event, deltaPos);
  }

  template <class E>
  static void
  shiftPositionTo(std::shared_ptr<E> event, const double deltaTime)
  {
    SimulatedEvent<E>::shiftPositionTo(event, deltaTime);
  }

  ///
  /// Getter
  ///

  //~ /// @brief Getter of the unit of momentum used
  //~ /// @return Unit in type of Acts::units
  template <class E>
  static double
  momentumUnit(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::momentumUnit(E * event);
  }

  //~ /// @brief Getter of the unit of length used
  //~ /// @return Unit in type of Acts::units
  template <class E>
  static double
  lengthUnit(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::lengthUnit(E * event);
  }

  /// @brief Getter of the position of the vertex
  /// @return Vector to the location of the vertex
  template <class E>
  static Acts::Vector3D
  eventPos(std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::eventPos(E * event);
  }

  /// @brief Getter of the time of the vertex
  /// @return Time of the vertex
  template <class E>
  static double
  eventTime(std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::eventTime(E * event);
  }

  /// @brief Get list of const particles
  /// @return List of const particles
  template <class E>
  static std::vector<std::unique_ptrActs::ParticleProperties>>
  particles(std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::particles(E * event);
  }

  /// @brief Get list of const vertices
  /// @return List of const vertices
  template <class E>
  static std::vector<std::unique_ptr<Acts::ProcessVertex>
  vertices(std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::vertices(E * event);
  }

  /// @brief Get beam particles
  /// @return List of beam particles
  template <class E>
  static std::vector<std::unique_ptr<Acts::ParticleProperties>>
  beams(std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::beams(E * event);
  }

  ///
  /// Adder
  ///

  //~ /// @brief Adds a new particle
  //~ /// @param particle new particle that will be added
  //~ /// @param mass mass of the new particle
  //~ /// @param status HepMC internal steering of the particle's behaviour
  template <class E>
  static void
  addParticle(std::shared_ptr<E> event, std::shared_ptr<Acts::ParticleProperties> particle)
  {
	  SimulatedEvent<E>::addParticle(event, particle);
  }

  //~ /// @brief Adds a new vertex
  //~ /// @param vertex new vertex that will be added
  //~ /// @param statusVtx HepMC internal steering of the vertex' behaviour
  //~ /// @param statusIn HepMC internal steering of the behaviour of incoming
  //~ /// particles
  //~ /// @param statusOut HepMC internal steering of the behaviour of outgoing
  //~ /// particles
  //~ /// @note The statuses are not represented in Acts and therefore need to
  //~ /// be added manually.
  template <class E>
  static void
  addVertex(const std::shared_ptr<Acts::ProcessVertex> vertex);

  /// @brief Adds a tree of particles and corresponding vertices to the
  //~ //record.
  /// @note This function needs vertices since in Acts only the vertices
  //~ //know
  /// the particles that enter/exit a vertex. HepMC propagates this
  //~ //information
  /// to the particles, too. Therefore the Acts vertices need to be
  //~ //translated
  /// into corresponding HepMC::GenParticles.
  /// @param actsVertices list of vertices that will be added. These
  //~ //vertices
  /// contain the participating particles.
  /// @param statusVtx HepMC internal steering of the vertex' behaviour
  /// @param statusIn HepMC internal steering of the behaviour of incoming
  /// particles
  /// @param statusOut HepMC internal steering of the behaviour of outgoing
  /// particles
  /// @note The statuses are not steering in Acts and therefore need to be
  /// added manually.
  template <class E>
  static void
  addTree(const std::vector<std::shared_ptr<Acts::ProcessVertex>>&
  //~ //actsVertices,
  const std::vector<int>                                   statusVtx,
  const std::map<barcode_type, int> statusIn,
  const std::map<barcode_type, int> statusOut);

  //~ ///
  //~ /// Remover
  //~ ///

  //~ /// @brief Removes a particle from the record
  //~ /// @param actsParticle particle that will be removed
  //~ template <class E>
  //~ static void
  //~ removeParticle(const std::shared_ptr<Acts::ParticleProperties>&
  //actsParticle);

  //~ /// @brief Removes multiple particles from the record
  //~ /// @param actsParticles particles that will be removed
  //~ template <class E>
  //~ static void
  //~ removeParticles(const
  //std::vector<std::shared_ptr<Acts::ParticleProperties>>&
  //~ actsParticles);

  //~ /// @brief Removes a vertex from the record
  //~ /// @note The identification of the vertex is potentially unstable (c.f.
  //~ /// HepMC3Event::compareVertices())
  //~ /// @param actsVertex vertex that will be removed
  //~ template <class E>
  //~ static void
  //~ removeVertex(const std::shared_ptr<Acts::ProcessVertex>& actsVertex);

}  // SimVertex
}  // FW
