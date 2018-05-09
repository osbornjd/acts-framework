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

#ifdef USE_HEPMC3
#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"
#endif

namespace FW {

/// @struct SimulatedEvent
///
/// This structure is the default structure for events from external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class E>
struct SimulatedEvent
{
};

namespace SimEvent {

  ///
  /// Setter
  ///

  /// @brief Sets new units for momentums
  /// @param event event in external data type
  /// @param momentumUnit new unit of momentum
  template <class E>
  static void
  momentumUnit(std::shared_ptr<E> event, const double momentumUnit)
  {
    SimulatedEvent<E>::setMomentumUnit(event, momentumUnit);
  }

  /// @brief Sets new units for lengths
  /// @param event event in external data type
  /// @param lengthUnit new unit of length
  template <class E>
  static void
  lengthUnit(std::shared_ptr<E> event, const double lengthUnit)
  {
    SimulatedEvent<E>::setLengthUnit(event, lengthUnit);
  }

  /// @brief Shifts the positioning of an event in space and time
  /// @param event event in external data type
  /// @param deltaPos relative spatial shift that will be applied
  /// @param deltaTime relative time shift that will be applied
  template <class E>
  static void
  shiftPositionBy(std::shared_ptr<E>    event,
                  const Acts::Vector3D& deltaPos,
                  const double          deltaTime)
  {
    SimulatedEvent<E>::shiftPositionBy(event, deltaPos, deltaTime);
  }

  /// @brief Shifts the positioning of an event to a paint in space and time
  /// @param event event in external data type
  /// @param pos new position of the event
  /// @param time new time of the event
  template <class E>
  static void
  shiftPositionTo(std::shared_ptr<E>    event,
                  const Acts::Vector3D& pos,
                  const double          time)
  {
    SimulatedEvent<E>::shiftPositionTo(event, pos, time);
  }

  /// @brief Shifts the positioning of an event to a paint in space
  /// @param event event in external data type
  /// @param pos new position of the event
  template <class E>
  static void
  shiftPositionTo(std::shared_ptr<E> event, const Acts::Vector3D& pos)
  {
    SimulatedEvent<E>::shiftPositionTo(event, pos);
  }

  /// @brief Shifts the positioning of an event to a paint in time
  /// @param event event in external data type
  /// @param time new time of the event
  template <class E>
  static void
  shiftPositionTo(std::shared_ptr<E> event, const double time)
  {
    SimulatedEvent<E>::shiftPositionTo(event, time);
  }

  ///
  /// Adder
  ///

  /// @brief Adds a new particle
  /// @param event event in external data type
  /// @param particle new particle that will be added
  template <class E>
  static void
  addParticle(std::shared_ptr<E>                        event,
              std::shared_ptr<Acts::ParticleProperties> particle)
  {
    SimulatedEvent<E>::addParticle(event, particle);
  }

  /// @brief Adds a new vertex
  /// @param event event in external data type
  /// @param vertex new vertex that will be added
  template <class E>
  static void
  addVertex(std::shared_ptr<E>                         event,
            const std::shared_ptr<Acts::ProcessVertex> vertex)
  {
    SimulatedEvent<E>::addVertex(event, vertex);
  }

  ///
  /// Remover
  ///

  /// @brief Removes a particle from the record
  /// @param event event in external data type
  /// @param particle particle that will be removed
  template <class E>
  static void
  removeParticle(std::shared_ptr<E>                               event,
                 const std::shared_ptr<Acts::ParticleProperties>& particle)
  {
    SimulatedEvent<E>::removeParticle(event, particle);
  }

  /// @brief Removes a vertex from the record
  /// @param event event in external data type
  /// @param vertex vertex that will be removed
  template <class E>
  static void
  removeVertex(std::shared_ptr<E>                          event,
               const std::shared_ptr<Acts::ProcessVertex>& vertex)
  {
    SimulatedEvent<E>::removeVertex(event, vertex);
  }

  ///
  /// Getter
  ///

  /// @brief Getter of the unit of momentum used
  /// @param event event in external data type
  /// @return unit of momentum
  template <class E>
  static double
  momentumUnit(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::momentumUnit(event);
  }

  /// @brief Getter of the unit of length used
  /// @param event event in external data type
  /// @return unit of length
  template <class E>
  static double
  lengthUnit(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::lengthUnit(event);
  }

  /// @brief Getter of the position of the event
  /// @param event event in external data type
  /// @return vector to the location of the event
  template <class E>
  static Acts::Vector3D
  eventPos(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::eventPos(event);
  }

  /// @brief Getter of the time of the event
  /// @param event event in external data type
  /// @return time of the event
  template <class E>
  static double
  eventTime(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::eventTime(event);
  }

  /// @brief Get list of particles
  /// @param event event in external data type
  /// @return List of particles
  template <class E>
  static std::vector<std::unique_ptr<Acts::ParticleProperties>>
  particles(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::particles(event);
  }

  /// @brief Get list of vertices
  /// @param event event in external data type
  /// @return List of vertices
  template <class E>
  static std::vector<std::unique_ptr<Acts::ProcessVertex>>
  vertices(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::vertices(event);
  }

  /// @brief Get beam particles
  /// @param event event in external data type
  /// @return List of beam particles
  template <class E>
  static std::vector<std::unique_ptr<Acts::ParticleProperties>>
  beams(const std::shared_ptr<E> event)
  {
    return SimulatedEvent<E>::beams(event);
  }
}  // SimEvent
}  // FW
