// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Plugins/HepMC/SimVertex.hpp"
#include "HepMC/FourVector.h"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepPID/ParticleIDMethods.hh"

namespace FW {

//~ /// @class HepMC3Event
//~ ///
//~ /// This class inherits from HepMC::GenEvent and expands the base class by
//~ /// setter and getter functions that take or return parameters such as
//vertices
//~ /// or particles as Acts objects. The internal behaviour of HepMC::GenEvent
//is
//~ /// not changed by this class.
//~ ///
template <>
struct SimulatedVertex<HepMC::GenVertex>
{

  static std::unique_ptr<Acts::ProcessVertex>
  processVertex(const HepMC::GenVertex* vertex);

  static bool
  inEvent(const HepMC::GenVertex* vertex);

  static int
  id(const HepMC::GenVertex* vertex);

  static void
  addParticleIn(HepMC::GenVertex* vertex, Acts::ParticleProperties* particle);

  static void
  addParticleOut(HepMC::GenVertex* vertex, Acts::ParticleProperties* particle);

  static void
  removeParticleIn(HepMC::GenVertex*         vertex,
                   Acts::ParticleProperties* particle);

  static void
  removeParticleOut(HepMC::GenVertex*         vertex,
                    Acts::ParticleProperties* particle);

  static std::vector<Acts::ParticleProperties>
  particlesIn(const HepMC::GenVertex* vertex);

  static std::vector<Acts::ParticleProperties>
  particlesOut(const HepMC::GenVertex* vertex);

  static Acts::Vector3D
  position(const HepMC::GenVertex* vertex);

  static double
  time(const HepMC::GenVertex* vertex);
};
}  // FW
