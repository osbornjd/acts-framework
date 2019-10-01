// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <Acts/Geometry/GeometryID.hpp>
#include <Acts/Surfaces/Surface.hpp>
#include <Acts/Utilities/Definitions.hpp>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimParticle.hpp"

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
    /// Store a geometry id copy to avoid indirection through the surface.
    Acts::GeometryID geometryId;
    /// The surface where the hit was created. Using a pointer enables default
    /// copying and assignment constructors/operators.
    /// TODO the geometry id should be sufficient to identify the surface
    const Acts::Surface* surface;
    /// The particle that created the simulation hit.
    /// TODO can this be replaced by just the barcode?
    SimParticle particle;

    /// A hit must be constructed with a valid surface.
    SimHit(const Acts::Surface& s) : geometryId(s.geoID()), surface(&s) {}
    SimHit() = delete;

    /// Geometry id accessor for compatibility with `GeometryIdMultiset`.
    Acts::GeometryID
    geoId() const
    {
      return geometryId;
    }
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

using SimHits = GeometryIdMultiset<Data::SimHit>;

}  // end of namespace FW
