// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>

#include <Acts/Utilities/Definitions.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/SimParticle.hpp"

namespace FW {

/// Value type to identify which process was used to generate the vertex.
using process_code = unsigned int;

namespace Data {

  /// Vertex information for physics process samplers.
  ///
  /// All quatities are calculated at first construction as they may  be used by
  /// downstream samplers
  ///
  /// @note If a sampler changes one of the parameters, consistency
  ///       can be broken, so it should update the rest (no checking done)
  struct SimVertex
  {
    /// The vertex position
    Acts::Vector3D position = Acts::Vector3D(0., 0., 0.);
    /// An optional time stamp
    double timeStamp = 0.;
    /// An optional process code
    process_code processCode = 9;
    /// The ingoing particles in the vertex
    std::vector<SimParticle> in = {};
    /// The outgoing particles from the vertex
    std::vector<SimParticle> out = {};

    /// Default
    SimVertex() = default;
    /// @brief Construct a particle consistently
    ///
    /// @param ertex The vertex position
    /// @param in The ingoing particles - copy
    /// @param out The outgoing particles (copy - can we do a move ?)
    /// @param vprocess The process code
    /// @param time The time stamp of this vertex
    SimVertex(const Acts::Vector3D&           vertex,
              const std::vector<SimParticle>& ingoing  = {},
              std::vector<SimParticle>        outgoing = {},
              process_code                    process  = 0,
              double                          time     = 0.)
      : position(vertex)
      , in(ingoing)
      , out(outgoing)
      , processCode(process)
      , timeStamp(time)
    {
    }

    /// Forward the particle access to the outgoing particles: begin
    auto
    outgoing_begin()
    {
      return out.begin();
    }

    /// Forward the particle access to the outgoing particles: end
    auto
    outgoing_end()
    {
      return out.end();
    }

    // Outgoing particles
    const std::vector<SimParticle>&
    outgoing() const
    {
      return out;
    }

    /// Forward the particle access to the outgoing particles: insert
    ///
    /// @param inparticles are the particles to be inserted
    auto
    outgoing_insert(const std::vector<SimParticle>& inparticles)
    {
      return out.insert(out.end(), inparticles.begin(), inparticles.end());
    }
  };
}  // end of namespace Data
}  // end of namespace FW
