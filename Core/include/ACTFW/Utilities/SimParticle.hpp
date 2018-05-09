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

#include "ACTFW/Plugins/HepMC/HepMC3Particle.hpp"

namespace FW {

/// @struct SimulatedParticle
///
/// This structure is the default structure for particles from external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class T>
struct SimulatedParticle
{
};

namespace SimParticle {

  /// @brief Returns a particle translated into Acts
  /// @param particle particle in external data type
  /// @return corresponding Acts particle
  template <class T>
  static std::unique_ptr<Acts::ParticleProperties>
  particleProperties(const std::shared_ptr<T> particle)
  {
    return std::move(SimulatedParticle<T>::particleProperties(particle));
  }

  /// @brief Returns the id of the particle translated into Acts
  /// @param particle particle in external data type
  /// @return id of the particle
  template <class T>
  static int
  id(const std::shared_ptr<T> particle)
  {
    return SimulatedParticle<T>::id(particle);
  }

  /// @brief Returns the production vertex of the particle translated into Acts
  /// @param particle particle in external data type
  /// @return production vertex of the particle or nullptr if it does not exist
  template <class T>
  std::unique_ptr<Acts::ProcessVertex>
  productionVertex(const std::shared_ptr<T> particle)
  {
    return std::move(SimulatedParticle<T>::productionVertex(particle));
  }

  /// @brief Returns the end vertex of the particle translated into Acts
  /// @param particle particle in external data type
  /// @return end vertex of the particle or nullptr if it does not exist
  template <class T>
  std::unique_ptr<Acts::ProcessVertex>
  endVertex(const std::shared_ptr<T> particle)
  {
    return std::move(SimulatedParticle<T>::endVertex(particle));
  }

  /// @brief Returns the PDG code of a particle translated into Acts
  /// @param particle particle in external data type
  /// @return PDG code of the particle
  template <class T>
  int
  pdgID(const std::shared_ptr<T> particle)
  {
    return SimulatedParticle<T>::pdgID(particle);
  }

  /// @brief Returns the momentum of a particle translated into Acts
  /// @param particle particle in external data type
  /// @return momentum of the particle
  template <class T>
  Acts::Vector3D
  momentum(const std::shared_ptr<T> particle)
  {
    return SimulatedParticle<T>::momentum(particle);
  }

  /// @brief Returns the energy of a particle translated into Acts
  /// @param particle particle in external data type
  /// @return energy of the particle
  template <class T>
  double
  energy(const std::shared_ptr<T> particle)
  {
    return SimulatedParticle<T>::energy(particle);
  }

  /// @brief Returns the mass of a particle translated into Acts
  /// @param particle particle in external data type
  /// @return mass of the particle
  template <class T>
  double
  mass(const std::shared_ptr<T> particle)
  {
    return SimulatedParticle<T>::mass(particle);
  }

  /// @brief Returns the charge of a particle translated into Acts
  /// @param particle particle in external data type
  /// @return charge of the particle
  template <class T>
  double
  charge(const std::shared_ptr<T> particle)
  {
    return SimulatedParticle<T>::charge(particle);
  }

  /// @brief Sets the PDG code of a particle translated from Acts
  /// @param particle particle in external data type
  /// @param pid PDG code that will be set
  template <class T>
  void
  pdgID(std::shared_ptr<T> particle, const int pid)
  {
    SimulatedParticle<T>::pdgID(particle, pid);
  }

  /// @brief Sets the momentum of a particle translated from Acts
  /// @param particle particle in external data type
  /// @param mom momentum that will be set
  template <class T>
  void
  momentum(std::shared_ptr<T> particle, const Acts::Vector3D& mom)
  {
    SimulatedParticle<T>::momentum(particle, mom);
  }

  /// @brief Sets the energy of a particle translated from Acts
  /// @param particle particle in external data type
  /// @param energy energy that will be set
  template <class T>
  void
  energy(std::shared_ptr<T> particle, const double energy)
  {
    SimulatedParticle<T>::momentum(particle, energy);
  }

  /// @brief Sets the mass of a particle translated from Acts
  /// @param particle particle in external data type
  /// @param mass mass that will be set
  template <class T>
  void
  mass(std::shared_ptr<T> particle, const double mass)
  {
    SimulatedParticle<T>::mass(particle, mass);
  }

}  // SimParticle
}  // FW
