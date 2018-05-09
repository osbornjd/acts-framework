// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Utilities/SimVertex.hpp"
#include "HepMC/FourVector.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"

namespace FW {

/// @struct HepMC3Vertex
///
/// This struct is an explicit implementation of FW::SimulatedVertex for the
/// translation of HepMC::GenVertex objects into Acts.
///
template <>
struct SimulatedVertex<HepMC::GenVertex>
{
  /// @brief Returns a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @return corresponding Acts vertex
  static std::unique_ptr<Acts::ProcessVertex>
  processVertex(const HepMC::GenVertex* vertex);

  /// @brief Returns a boolean expression if a vertex is in an event translated
  /// into Acts
  /// @param vertex vertex in HepMC data type
  /// @return boolean expression if the vertex is in an event
  static bool
  inEvent(const HepMC::GenVertex* vertex);

  /// @brief Returns a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @return id of the vertex
  static int
  id(const HepMC::GenVertex* vertex);

  /// @brief Adds an incoming particle to a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @param particle incoming particle that will be added
  static void
  addParticleIn(HepMC::GenVertex* vertex, Acts::ParticleProperties* particle);

  /// @brief Adds an outgoing particle to a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @param particle outgoing particle that will be added
  static void
  addParticleOut(HepMC::GenVertex* vertex, Acts::ParticleProperties* particle);

  /// @brief Removes an incoming particle from a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @param particle incoming particle that will be removed
  static void
  removeParticleIn(HepMC::GenVertex*         vertex,
                   Acts::ParticleProperties* particle);

  /// @brief Removes an outgoing particle from a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @param particle outgoing particle that will be removed
  static void
  removeParticleOut(HepMC::GenVertex*         vertex,
                    Acts::ParticleProperties* particle);

  /// @brief Returns the incoming particles of a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @return incoming particles of the vertex
  static std::vector<Acts::ParticleProperties>
  particlesIn(const HepMC::GenVertex* vertex);

  /// @brief Returns the outgoing particles of a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @return outgoing particles of the vertex
  static std::vector<Acts::ParticleProperties>
  particlesOut(const HepMC::GenVertex* vertex);

  /// @brief Returns the position of a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @return position of the vertex
  static Acts::Vector3D
  position(const HepMC::GenVertex* vertex);

  /// @brief Returns the time of a vertex translated into Acts
  /// @param vertex vertex in HepMC data type
  /// @return time of the vertex
  static double
  time(const HepMC::GenVertex* vertex);

private:
  /// @brief Converts HepMC::GenParticle objects into Acts
  /// @param genParticles list of HepMC::GenParticle objects
  /// @return converted list
  static std::vector<Acts::ParticleProperties>
  genParticlesToActs(const std::vector<HepMC::GenParticlePtr>& genParticles);

  /// @brief Converts an Acts::ParticleProperties into HepMC::GenParticle
  /// @param actsParticle Acts particle that will be converted
  /// @return converted particle
  static HepMC::GenParticlePtr
  actsParticleToGen(Acts::ParticleProperties* actsParticle);

  /// @brief Finds a HepMC::GenParticle from a list that matches an
  /// Acts::ParticleProperties object
  /// @param genParticles list of HepMC particles
  /// @param actsParticle Acts particle
  /// @return HepMC particle that matched with the Acts particle or nullptr if
  /// no match was found
  static HepMC::GenParticlePtr
  matchParticles(const std::vector<HepMC::GenParticlePtr>& genParticles,
                 Acts::ParticleProperties*                 actsParticle);
};
}  // FW
