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
#include "ACTFW/Plugins/HepMC/HepMC3Vertex.hpp"

namespace FW {

/// @struct SimulatedVertex
///
/// This structure is the default structure for vertices from external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class V>
struct SimulatedVertex
{
};

namespace SimVertex {

  /// @brief Returns a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @return corresponding Acts vertex
  template <class V>
  static std::unique_ptr<Acts::ProcessVertex>
  processVertex(const V* vertex)
  {
    return std::move(SimulatedVertex<V>::processVertex(vertex));
  }

  /// @brief Returns a boolean expression if a vertex is in an event translated
  /// into Acts
  /// @param vertex vertex in external data type
  /// @return boolean expression if the vertex is in an event
  template <class V>
  static bool
  inEvent(const V* vertex)
  {
    return SimulatedVertex<V>::inEvent(vertex);
  }

  /// @brief Returns a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @return id of the vertex
  template <class V>
  static int
  id(const V* vertex)
  {
    return SimulatedVertex<V>::id(vertex);
  }

  /// @brief Returns the incoming particles of a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @return incoming particles of the vertex
  template <class V>
  static std::vector<Acts::ParticleProperties>
  particlesIn(const V* vertex)
  {
    return SimulatedVertex<V>::particlesIn(vertex);
  }

  /// @brief Returns the outgoing particles of a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @return outgoing particles of the vertex
  template <class V>
  static std::vector<Acts::ParticleProperties>
  particlesOut(const V* vertex)
  {
    return SimulatedVertex<V>::particlesOut(vertex);
  }

  /// @brief Returns the position of a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @return position of the vertex
  template <class V>
  static Acts::Vector3D
  position(const V* vertex)
  {
    return SimulatedVertex<V>::position(vertex);
  }

  /// @brief Returns the time of a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @return time of the vertex
  template <class V>
  static double
  time(const V* vertex)
  {
    return SimulatedVertex<V>::time(vertex);
  }

  /// @brief Adds an incoming particle to a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @param particle incoming particle that will be added
  template <class V>
  static void
  addParticleIn(V* vertex, Acts::ParticleProperties* particle)
  {
    SimulatedVertex<V>::addParticleIn(vertex, particle);
  }

  /// @brief Adds an outgoing particle to a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @param particle outgoing particle that will be added
  template <class V>
  static void
  addParticleOut(V* vertex, Acts::ParticleProperties* particle)
  {
    SimulatedVertex<V>::addParticleOut(vertex, particle);
  }

  /// @brief Removes an incoming particle from a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @param particle incoming particle that will be removed
  template <class V>
  static void
  removeParticleIn(V* vertex, Acts::ParticleProperties* particle)
  {
    SimulatedVertex<V>::removeParticleIn(vertex, particle);
  }

  /// @brief Removes an outgoing particle from a vertex translated into Acts
  /// @param vertex vertex in external data type
  /// @param particle outgoing particle that will be removed
  template <class V>
  static void
  removeParticleOut(V* vertex, Acts::ParticleProperties* particle)
  {
    SimulatedVertex<V>::removeParticleOut(vertex, particle);
  }
}  // SimVertex
}  // FW
