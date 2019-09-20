// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <Acts/Utilities/Definitions.hpp>

#include "ACTFW/EventData/SimParticle.hpp"

namespace Acts {
class Surface;
}
namespace FW {
namespace Data {

  /// A particle hit on a surface.
  ///
  /// This contains the minimal, undigitized information.
  struct SimHit
  {
    /// TODO replace by combined 4d position
    /// The global position of the hit
    Acts::Vector3D position = Acts::Vector3D(0., 0., 0.);
    /// The time of the hit
    double time = 0.;
    /// The global direction of the particle at hit position
    Acts::Vector3D direction = Acts::Vector3D(0., 0., 0.);
    /// The value representing the hit (e.g. energy deposit)
    double value = 0.;
    /// The surface where the hit was created
    const Acts::Surface* surface = nullptr;
    /// The particle that created the simulation hit
    SimParticle particle;
  };

  /// @brief Constructor of Sensitive Hits, provides a simple
  /// interface to construct simulation hits from Fatras
  struct SimHitCreator
  {
    /// @brief Hit creation from Fatras simulation
    ///
    /// @param surface is the Surface where the hit is created
    /// @param position is the global hit position
    /// @param direction is the momentum direction at hit position
    /// @param value the simulated value
    /// @param time is the timeStamp
    /// @param particle the particle for the truth link
    ///
    /// @return a SimHit object that is created from input parameters
    SimHit
    operator()(const Acts::Surface&  surface,
               const Acts::Vector3D& position,
               const Acts::Vector3D& direction,
               double                value,
               double                time,
               const SimParticle&    simParticle) const
    {
      SimHit simHit;
      simHit.position  = position;
      simHit.time      = time;
      simHit.direction = direction;
      simHit.value     = value;
      simHit.surface   = &surface;
      simHit.particle  = simParticle;
      return simHit;
    }
  };

}  // namespace Data
}  // end of namespace FW
