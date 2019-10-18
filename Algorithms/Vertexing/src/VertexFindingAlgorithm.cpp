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
FWE::VertexFindingAlgorithm::execute(const FW::AlgorithmContext& context) const
{

  using Propagator = Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>>;
  using Linearizer_t
      = Acts::HelicalTrackLinearizer<Acts::ConstantBField, Propagator>;

  using VertexFitter
      = Acts::FullBilloirVertexFitter<Acts::BoundParameters, Linearizer_t>;

  const auto& input = context.eventStore.get<std::vector<FW::VertexAndTracks>>(
      m_cfg.trackCollection);

  // Set up constant B-Field
  Acts::ConstantBField bField(m_cfg.bField);

  // Set up Eigenstepper
  Acts::EigenStepper<Acts::ConstantBField> stepper(bField);

  // Set up propagator with void navigator
  auto propagator = std::make_shared<Propagator>(stepper);

  // Set up track linearizer
  Acts::PropagatorOptions<Acts::ActionList<>, Acts::AbortList<>> pOptions
      = Linearizer_t::getDefaultPropagatorOptions(context.geoContext,
                                                  context.magFieldContext);

  Linearizer_t::Config ltConfig(bField, propagator, pOptions);
  Linearizer_t         linearizer(ltConfig);

  // Set up vertex fitter
  VertexFitter::Config vertexFitterCfg;
  VertexFitter         vertexFitter(vertexFitterCfg);

  // Set up all seed finder related things
  Acts::TrackToVertexIPEstimator<Acts::BoundParameters, Propagator>::Config
                                                                    ipEstCfg(propagator, pOptions);
  Acts::TrackToVertexIPEstimator<Acts::BoundParameters, Propagator> ipEst(
      ipEstCfg);

  using ZScanSeedFinder = Acts::ZScanVertexFinder<VertexFitter>;

  static_assert(Acts::VertexFinderConcept<ZScanSeedFinder>,
                "Vertex finder does not fulfill vertex finder concept.");

  ZScanSeedFinder::Config sFcfg(std::move(ipEst));

  ZScanSeedFinder sFinder(std::move(sFcfg));

  // Vertex Finder
  using VertexFinder
      = Acts::IterativeVertexFinder<VertexFitter, ZScanSeedFinder>;

  static_assert(Acts::VertexFinderConcept<VertexFinder>,
                "Vertex finder does not fulfill vertex finder concept.");

  VertexFinder::Config cfg(
      std::move(vertexFitter), std::move(linearizer), std::move(sFinder));

  cfg.maxVertices                 = 200;
  cfg.reassignTracksAfterFirstFit = true;

  VertexFinder vertexFinder(cfg);

  // Vertex finder options
  Acts::VertexFinderOptions<Acts::BoundParameters> vFinderOptions(
      context.geoContext, context.magFieldContext);

  std::vector<Acts::BoundParameters> inputTrackCollection;

  int counte = 0;
  for (auto& bla : input) { counte += bla.tracks.size(); }

  ACTS_INFO("Truth vertices in event: " << input.size());

  for (auto& vertexAndTracks : input) {
    ACTS_INFO("\t True vertex at ("
              << vertexAndTracks.vertex.position[0] << ","
              << vertexAndTracks.vertex.position[1] << ","
              << vertexAndTracks.vertex.position[2] << ") with "
              << vertexAndTracks.tracks.size() << " tracks.");
    inputTrackCollection.insert(inputTrackCollection.end(),
                                vertexAndTracks.tracks.begin(),
                                vertexAndTracks.tracks.end());
  }

  // Find vertices
  auto res = vertexFinder.find(inputTrackCollection, vFinderOptions);

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
