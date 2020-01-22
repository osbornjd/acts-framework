// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Vertexing/VertexFindingAlgorithm.hpp"
#include <Acts/Geometry/GeometryContext.hpp>
#include <Acts/MagneticField/MagneticFieldContext.hpp>
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/TruthTracking/VertexAndTracks.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Acts/Vertexing/FullBilloirVertexFitter.hpp"
#include "Acts/Vertexing/HelicalTrackLinearizer.hpp"
#include "Acts/Vertexing/ImpactPoint3dEstimator.hpp"
#include "Acts/Vertexing/IterativeVertexFinder.hpp"
#include "Acts/Vertexing/LinearizedTrack.hpp"
#include "Acts/Vertexing/TrackToVertexIPEstimator.hpp"
#include "Acts/Vertexing/Vertex.hpp"
#include "Acts/Vertexing/VertexFinderConcept.hpp"
#include "Acts/Vertexing/ZScanVertexFinder.hpp"

#include <iostream>

FWE::VertexFindingAlgorithm::VertexFindingAlgorithm(const Config&        cfg,
                                                    Acts::Logging::Level level)
  : FW::BareAlgorithm("VertexFinding", level), m_cfg(cfg)
{
}

/// @brief Algorithm that receives all selected tracks from an event
/// and finds and fits its vertices
FW::ProcessCode
FWE::VertexFindingAlgorithm::execute(const FW::AlgorithmContext& ctx) const
{
  using MagneticField     = Acts::ConstantBField;
  using Stepper           = Acts::EigenStepper<MagneticField>;
  using Propagator        = Acts::Propagator<Stepper>;
  using PropagatorOptions = Acts::PropagatorOptions<>;
  using TrackParameters   = Acts::BoundParameters;
  using Linearizer        = Acts::HelicalTrackLinearizer<Propagator>;
  using VertexFitter
      = Acts::FullBilloirVertexFitter<TrackParameters, Linearizer>;
  using VertexSeederImpactPointEstimator
      = Acts::TrackToVertexIPEstimator<TrackParameters, Propagator>;
  using VertexSeeder = Acts::ZScanVertexFinder<VertexFitter>;
  using VertexFinder = Acts::IterativeVertexFinder<VertexFitter, VertexSeeder>;
  using VertexFinderOptions = Acts::VertexFinderOptions<TrackParameters>;

  static_assert(Acts::VertexFinderConcept<VertexSeeder>,
                "VertexSeeder does not fulfill vertex finder concept.");
  static_assert(Acts::VertexFinderConcept<VertexFinder>,
                "VertexFinder does not fulfill vertex finder concept.");

  // Set up the magnetic field
  MagneticField bField(m_cfg.bField);
  // Set up propagator with void navigator
  auto              propagator = std::make_shared<Propagator>(Stepper(bField));
  PropagatorOptions propagatorOpts(ctx.geoContext, ctx.magFieldContext);
  // Setup the vertex fitter
  VertexFitter::Config vertexFitterCfg;
  VertexFitter         vertexFitter(std::move(vertexFitterCfg));
  // Setup the track linearizer
  Linearizer::Config linearizerCfg(bField, propagator, propagatorOpts);
  Linearizer         linearizer(std::move(linearizerCfg));
  // Setup the seed finder
  VertexSeederImpactPointEstimator::Config seederIpEstCfg(propagator,
                                                          propagatorOpts);
  VertexSeederImpactPointEstimator seederIpEst(std::move(seederIpEstCfg));
  VertexSeeder::Config             seederCfg(std::move(seederIpEst));
  VertexSeeder                     seeder(std::move(seederCfg));
  // Setup the impact point estimator
  VertexFinder::ImpactPointEstimator::Config finderIpEstCfg(
      bField, propagator, propagatorOpts);
  VertexFinder::ImpactPointEstimator finderIpEst(std::move(finderIpEstCfg));
  // Set up the actual vertex finder
  VertexFinder::Config finderCfg(std::move(vertexFitter),
                                 std::move(linearizer),
                                 std::move(seeder),
                                 std::move(finderIpEst));
  finderCfg.maxVertices                 = 200;
  finderCfg.reassignTracksAfterFirstFit = true;
  VertexFinder        finder(finderCfg);
  VertexFinderOptions finderOpts(ctx.geoContext, ctx.magFieldContext);

  // Setup containers
  const auto& input = ctx.eventStore.get<std::vector<FW::VertexAndTracks>>(
      m_cfg.trackCollection);
  std::vector<Acts::BoundParameters> inputTrackCollection;

  int counte = 0;
  for (auto& bla : input) { counte += bla.tracks.size(); }

  ACTS_INFO("Truth vertices in event: " << input.size());

  for (auto& vertexAndTracks : input) {
    ACTS_INFO("\t True vertex at ("
              << vertexAndTracks.vertexPosition[0] << ","
              << vertexAndTracks.vertexPosition[1] << ","
              << vertexAndTracks.vertexPosition[2] << ") with "
              << vertexAndTracks.tracks.size() << " tracks.");
    inputTrackCollection.insert(inputTrackCollection.end(),
                                vertexAndTracks.tracks.begin(),
                                vertexAndTracks.tracks.end());
  }

  // Find vertices
  auto res = finder.find(inputTrackCollection, finderOpts);

  if (res.ok()) {
    // Retrieve vertices found by vertex finder
    auto vertexCollection = *res;

    ACTS_INFO("Found " << vertexCollection.size() << " vertices in event.");

    unsigned int count = 0;
    for (const auto& vtx : vertexCollection) {
      ACTS_INFO("\t" << ++count << ". vertex at "
                     << "(" << vtx.position()[0] << "," << vtx.position()[1]
                     << "," << vtx.position()[2] << ") with "
                     << vtx.tracks().size() << " tracks.");
    }
  } else {
    ACTS_ERROR("Error in vertex finder.");
  }

  return FW::ProcessCode::SUCCESS;
}
