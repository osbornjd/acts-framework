// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Vertexing/VertexFitAlgorithm.hpp"
#include <iostream>

#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/TruthTracking/VertexAndTracks.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Vertexing/FullBilloirVertexFitter.hpp"
#include "Acts/Vertexing/HelicalTrackLinearizer.hpp"
#include "Acts/Vertexing/LinearizedTrack.hpp"
#include "Acts/Vertexing/Vertex.hpp"

FWE::VertexFitAlgorithm::VertexFitAlgorithm(const Config&        cfg,
                                            Acts::Logging::Level level)
  : FW::BareAlgorithm("VertexFit", level), m_cfg(cfg)
{
}

/// @brief Algorithm that receives a set of tracks belonging to a common
/// vertex and fits the associated vertex to it
FW::ProcessCode
FWE::VertexFitAlgorithm::execute(const FW::AlgorithmContext& context) const
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

  Acts::PropagatorOptions<Acts::ActionList<>, Acts::AbortList<>> pOptions
      = Linearizer_t::getDefaultPropagatorOptions(context.geoContext,
                                                  context.magFieldContext);

  Linearizer_t::Config ltConfig(bField, propagator, pOptions);
  Linearizer_t         linearizer(ltConfig);

  VertexFitter::Config vertexFitterCfg;

  VertexFitter vertexFitter(vertexFitterCfg);

  for (auto& vertexAndTracks : input) {

    auto& inputTrackCollection = vertexAndTracks.tracks;

    Acts::Vertex<Acts::BoundParameters> fittedVertex;
    if (!m_cfg.doConstrainedFit) {
      if (inputTrackCollection.size() < 2) { continue; }
      // Vertex fitter options
      Acts::VertexFitterOptions<Acts::BoundParameters> vfOptions(
          context.geoContext, context.magFieldContext);

      auto fitRes
          = vertexFitter.fit(inputTrackCollection, linearizer, vfOptions);
      if (fitRes.ok()) {
        fittedVertex = *fitRes;
      } else {
        ACTS_ERROR("Error in vertex fit.");
      }
    } else {
      // Vertex constraint
      Acts::Vertex<Acts::BoundParameters> theConstraint;

      theConstraint.setCovariance(m_cfg.constraintCov);
      theConstraint.setPosition(m_cfg.constraintPos);

      // Vertex fitter options
      Acts::VertexFitterOptions<Acts::BoundParameters> vfOptionsConstr(
          context.geoContext, context.magFieldContext, theConstraint);

      auto fitRes
          = vertexFitter.fit(inputTrackCollection, linearizer, vfOptionsConstr);
      if (fitRes.ok()) {
        fittedVertex = *fitRes;
      } else {
        ACTS_ERROR("Error in vertex fit.");
      }
    }

    ACTS_INFO("Fitted Vertex: "
              << "(" << fittedVertex.position()[0] << ","
              << fittedVertex.position()[1] << "," << fittedVertex.position()[2]
              << ")");
    ACTS_INFO("Truth Vertex: "
              << "(" << vertexAndTracks.vertex.position[0] << ","
              << vertexAndTracks.vertex.position[1] << ","
              << vertexAndTracks.vertex.position[2] << ")");
  }

  return FW::ProcessCode::SUCCESS;
}
