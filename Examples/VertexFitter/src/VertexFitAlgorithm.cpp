// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "VertexFitAlgorithm.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

#include "Acts/Vertexing/LinearizedTrack.hpp"
#include "Acts/Vertexing/LinearizedTrackFactory.hpp"
#include "Acts/Vertexing/Vertex.hpp"

#include <iostream>

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

  const auto& inputTrackCollection
      = context.eventStore.get<std::vector<Acts::BoundParameters>>(
          m_cfg.trackCollection);

  // Vertex constraint
  Acts::Vertex<Acts::BoundParameters> myConstraint;
  Acts::ActsSymMatrixD<3>             myCovMat;
  // Set some arbitrary large values on the main diagonal
  myCovMat(0, 0) = 30.;
  myCovMat(1, 1) = 30.;
  myCovMat(2, 2) = 30.;
  myConstraint.setCovariance(std::move(myCovMat));
  myConstraint.setPosition(Acts::Vector3D(0, 0, 0));

  // Vertex fitter options
  Acts::VertexFitterOptions<Acts::BoundParameters> vfOptions(
      context.geoContext, context.magFieldContext);

  Acts::VertexFitterOptions<Acts::BoundParameters> vfOptionsConstr(
      context.geoContext, context.magFieldContext, myConstraint);

  Acts::Vertex<Acts::BoundParameters> fittedVertex;
  if (!m_cfg.doConstrainedFit) {
    auto fitRes = m_cfg.vertexFitter->fit(inputTrackCollection, vfOptions);
    if (fitRes.ok()) {
      fittedVertex = *fitRes;
    } else {
      ACTS_ERROR("Error in vertex fit.");
    }
  } else {

    auto fitRes
        = m_cfg.vertexFitter->fit(inputTrackCollection, vfOptionsConstr);
    if (fitRes.ok()) {
      fittedVertex = *fitRes;
    } else {
      ACTS_ERROR("Error in vertex fit.");
    }
  }

  ACTS_INFO("Fitted Vertex: "
            << "("
            << fittedVertex.position()[0]
            << ","
            << fittedVertex.position()[1]
            << ","
            << fittedVertex.position()[2]
            << ")");

  return FW::ProcessCode::SUCCESS;
}
