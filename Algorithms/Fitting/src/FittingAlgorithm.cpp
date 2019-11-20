// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Fitting/FittingAlgorithm.hpp"

#include <iostream>
#include <map>
#include <random>
#include <stdexcept>

#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/ParameterDefinitions.hpp"

FW::FittingAlgorithm::FittingAlgorithm(Config cfg, Acts::Logging::Level level)
  : FW::BareAlgorithm("FittingAlgorithm", level), m_cfg(std::move(cfg))
{
  if (m_cfg.inputSourceLinks.empty()) {
    throw std::invalid_argument("Missing input source links collection");
  }
  if (m_cfg.inputProtoTracks.empty()) {
    throw std::invalid_argument("Missing input proto tracks collection");
  }
  if (m_cfg.inputInitialTrackParameters.empty()) {
    throw std::invalid_argument(
        "Missing input initial track parameters collection");
  }
  if (m_cfg.outputTrajectories.empty()) {
    throw std::invalid_argument("Missing output trajectories collection");
  }
}

FW::ProcessCode
FW::FittingAlgorithm::execute(const FW::AlgorithmContext& context) const
{
  // read input data
  const auto sourceLinks
      = ctx.eventStore.get<SimSourceLinkContainer>(m_cfg.inputSourceLinks);
  const auto protoTracks
      = ctx.eventStore.get<ProtoTrackContainer>(m_cfg.inputProtoTracks);
  const auto initialParameters = ctx.eventStore.get<TrackParametersContainer>(
      m_cfg.inputInitialTrackParameters);

  // consistency cross checks
  if (protoTracks.size() != initialParameters.size()) {
    ACTS_FATAL("Inconsistent number of proto tracks and parameters");
    return ProcessCode::ABORT;
  }
  ACTS_DEBUG("Received " << protoTracks.size() << " proto tracks to be fitted");

  // prepare output data
  TrajectoryContainer trajectories;
  trajectories.reserve(protoTracks.size());

  // perform the fit for each input track
  std::vector<Data::SimSourceLink> trackSourceLinks;
  for (std::size_t itrack = 0; itrack < protoTracks.size(); ++itrack) {
    const auto& protoTrack    = protoTracks[itrack];
    const auto& initialParams = initialParameters[itrack];

    // prepare source links for this track
    trackSourceLinks.clear();
    for (auto hitIndex : protoTrack) {
      trackSourceLinks.push_back(sourceLinks.nth(hitIndex));
    }

    // setup the Kalman fitter
    Acts::KalmanFitterOptions fitterOptions(ctx.geoContext,
                                            ctx.magFieldContext,
                                            ctx.calibContext,
                                            &initialParams.referenceSurface());
    auto result = m_cfg.fit(trackSourceLinks, initialParams, fitterOptions);
    // store the results. always store something even if the states are empty
    // (bad fit) so the number of output trajectories is consistent w/ the
    // number of input proto tracks
    if (result.fittedParameters) {
      const auto& params = result.fittedParameters.get();
      ACTS_DEBUG("Fitted paramemeter for track " << itrack);
      ACTS_DEBUG("  position: " << params.position().transpose());
      ACTS_DEBUG("  momentum: " << params.momentum().transpose());
    } else {
      ACTS_WARNING("No fitted parameters for track track " << itrack);
    }

    // store the results. always store something even if the states are empty
    // (bad fit) so the number of output tracks is consistent w/ the number of
    // input proto tracks
    fittedTrajectories.emplace_back(std::move(result.fittedStates));
  }

  ctx.eventStore.add(m_cfg.outputTrajectories, std::move(fittedTrajectories));
  return FW::ProcessCode::SUCCESS;
}
