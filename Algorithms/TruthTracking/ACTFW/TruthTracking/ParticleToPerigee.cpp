// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/TruthTracking/ParticleToPerigee.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/Track.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"

using namespace FW;

ParticleToPerigee::ParticleToPerigee(const Config&        cfg,
                                     Acts::Logging::Level lvl)
  : BareAlgorithm("ParticleToPerigee", lvl), m_cfg(cfg)
{
  if (m_cfg.inputParameters.empty()) {
    throw std::invalid_argument("Missing input truth particles collection");
  }
  if (m_cfg.outputPerigee.empty()) {
    throw std::invalid_argument("Missing output perigee collection");
  }
}

ProcessCode
ParticleToPerigee::execute(const AlgorithmContext& ctx) const
{

  std::shared_ptr<Acts::PerigeeSurface> perigeeSurface
      = Acts::Surface::makeShared<Acts::PerigeeSurface>(m_cfg.refPosition);
  // Set up constant B-Field
  Acts::ConstantBField bField(m_cfg.bField);
  // Set up stepper
  Acts::EigenStepper<Acts::ConstantBField> stepper(bField);
  // Set up propagator with void navigator
  Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(
      stepper);
  // Set up propagator options
  Acts::PropagatorOptions<> pOptions(ctx.geoContext, ctx.magFieldContext);
  pOptions.direction = Acts::backward;

  // prepare input collections
  const auto& params
      = ctx.eventStore.get<TrackParametersContainer>(m_cfg.inputParameters);

  std::vector<Acts::BoundParameters> tracks;
  tracks.reserve(params.size());

  // Create perigee parameters for all input particles
  for (const auto& param : params) {

    // Run propagator
    auto result = propagator.propagate(param, *perigeeSurface, pOptions);
    if (!result.ok()) { continue; }

    // get perigee parameters
    const auto& perigeeParameters = (*result).endParameters->parameters();

    const auto& covMat = param.covariance();

    tracks.push_back(Acts::BoundParameters(
        ctx.geoContext, covMat, perigeeParameters, perigeeSurface)

    );
  }

  ctx.eventStore.add(m_cfg.outputPerigee, std::move(tracks));
  return ProcessCode::SUCCESS;
}
