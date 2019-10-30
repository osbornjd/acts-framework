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

  /// A simultated vertex e.g. from a physics process.
  struct SimVertex
  {
    /// TODO replace by combined 4d position
    /// The vertex position
    Acts::Vector3D position = Acts::Vector3D(0., 0., 0.);
    /// An optional time stamp
    double time = 0.;
    /// The incoming particles into the vertex
    std::vector<SimParticle> incoming = {};
    /// The outgoing particles from the vertex
    std::vector<SimParticle> outgoing = {};
    /// An optional process code
    process_code processCode = 9;

    SimVertex() = default;
    /// @param position_ The vertex position
    /// @param incoming_ The ingoing particles
    /// @param outgoing_ The outgoing particles
    /// @param process   The process code
    /// @param time      The vertex time
    SimVertex(const Acts::Vector3D&    position_,
              std::vector<SimParticle> incoming_ = {},
              std::vector<SimParticle> outgoing_ = {},
              process_code             process   = 0,
              double                   time      = 0.)
      : position(position_)
      , time(time)
      , incoming(std::move(incoming_))
      , outgoing(std::move(outgoing_))
      , processCode(process)
    {
    }

    // @todo let fatras use the member variables directly and remove the
    //       duplicated accessors below.

    /// Forward the particle access to the outgoing particles: begin
    auto
    outgoing_begin()
    {
      return outgoing.begin();
    }
    /// Forward the particle access to the outgoing particles: end
    auto
    outgoing_end()
    {
      return outgoing.end();
    }
    /// Forward the particle access to the outgoing particles: insert
    ///
    /// @param inparticles are the particles to be inserted
    auto
    outgoing_insert(const std::vector<SimParticle>& inparticles)
    {
      return outgoing.insert(
          outgoing.end(), inparticles.begin(), inparticles.end());
    }
  };
}  // end of namespace Data
}  // end of namespace FW
