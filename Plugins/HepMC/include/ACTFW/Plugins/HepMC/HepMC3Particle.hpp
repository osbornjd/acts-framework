// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Plugins/HepMC/HepMC3Vertex.hpp"
#include "ACTFW/Plugins/HepMC/SimParticle.hpp"
#include "HepMC/FourVector.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepPID/ParticleIDMethods.hh"

namespace FW {

/// @struct HepMC3Particle
///
/// This struct is an explicit implementation of FW::SimulatedParticle for the
/// translation of HepMC::GenParticle objects into Acts.
///
template <>
struct SimulatedParticle<HepMC::GenParticle>
{
  /// @brief Returns the particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return corresponding Acts particle
  static std::unique_ptr<Acts::ParticleProperties>
  particleProperties(const HepMC::GenParticle* particle);

  /// @brief Returns the id of the particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return id of the particle
  static int
  id(const HepMC::GenParticle* particle);

  /// @brief Returns the production vertex of the particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return production vertex of the particle
  static std::unique_ptr<Acts::ProcessVertex>
  productionVertex(const HepMC::GenParticle* particle);

  /// @brief Returns the end vertex of the particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return end vertex of the particle
  static std::unique_ptr<Acts::ProcessVertex>
  endVertex(const HepMC::GenParticle* particle);

  /// @brief Returns the PDG code of a particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return PDG code of the particle
  static int
  pdgID(const HepMC::GenParticle* particle);

  /// @brief Returns the momentum of a particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return momentum of the particle
  static Acts::Vector3D
  momentum(const HepMC::GenParticle* particle);

  /// @brief Returns the energy of a particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return energy of the particle
  static double
  energy(const HepMC::GenParticle* particle);

  /// @brief Returns the mass of a particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return mass of the particle
  static double
  mass(const HepMC::GenParticle* particle);

  /// @brief Returns the charge of a particle translated into Acts
  /// @param particle HepMC::GenParticle particle
  /// @return charge of the particle
  static double
  charge(const HepMC::GenParticle* particle);

  /// @brief Sets the PDG code of a particle translated from Acts
  /// @param particle HepMC::GenParticle particle
  /// @param pid PDG code that will be set
  static void
  pdgID(HepMC::GenParticle* particle, const int pid);

  /// @brief Sets the momentum of a particle translated from Acts
  /// @param particle HepMC::GenParticle particle
  /// @param mom momentum that will be set
  static void
  momentum(HepMC::GenParticle* particle, const Acts::Vector3D& mom);

  /// @brief Sets the energy of a particle translated from Acts
  /// @param particle HepMC::GenParticle particle
  /// @param energy energy that will be set
  static void
  energy(HepMC::GenParticle* particle, const double energy);

  /// @brief Sets the mass of a particle translated from Acts
  /// @param particle HepMC::GenParticle particle
  /// @param mass mass that will be set
  static void
  mass(HepMC::GenParticle* particle, const double mass);
};
}  // FW
