// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief All track-related shared types.

#pragma once

#include <vector>

#include <Acts/EventData/TrackParameters.hpp>
#include <Acts/EventData/TrackState.hpp>

#include "ACTFW/EventData/SimSourceLink.hpp"

namespace FW {

/// (Reconstructed) track parameters e.g. close to the vertex.
using TrackParameters = Acts::CurvilinearParameters;
/// Container of reconstructed track states for multiple tracks.
using TrackParametersContainer = std::vector<TrackParameters>;

/// A bound state on a surface w/ associated measurements.
using TrajectoryState
    = Acts::TrackState<Data::SimSourceLink, Acts::BoundParameters>;
/// A trajectory is a set of track states along a reconstructed path.
using Trajectory = std::vector<TrajectoryState>;
/// Container of reconstructed trajectories.
using TrajectoryContainer = std::vector<Trajectory>;

}  // namespace FW
