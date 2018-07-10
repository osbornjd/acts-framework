// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"

#include <iostream>
#include <random>
#include <stdexcept>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"
#include "Acts/Layers/Layer.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Units.hpp"

FW::ExtrapolationAlgorithm::ExtrapolationAlgorithm(
    const FW::ExtrapolationAlgorithm::Config& cfg,
    Acts::Logging::Level                      loglevel)
  : FW::BareAlgorithm("ExtrapolationAlgorithm", loglevel), m_cfg(cfg)
{
  if (!m_cfg.extrapolationEngine) {
    throw std::invalid_argument("Missing extrapolation engine");
  }
}

FW::ProcessCode
FW::ExtrapolationAlgorithm::execute(FW::AlgorithmContext ctx) const
{

  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);

  // output: the extrapolation cell collections
  std::vector<Acts::ExtrapolationCell<Acts::TrackParameters>>   cCells;
  std::vector<Acts::ExtrapolationCell<Acts::NeutralParameters>> nCells;

  // Create an algorithm local random number generator
  RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(ctx);

  // Setup random number distributions for some quantities
  FW::GaussDist   d0Dist(0., m_cfg.d0Sigma);
  FW::GaussDist   z0Dist(0., m_cfg.z0Sigma);
  FW::UniformDist phiDist(m_cfg.phiRange.first, m_cfg.phiRange.second);
  FW::UniformDist etaDist(m_cfg.etaRange.first, m_cfg.etaRange.second);
  FW::UniformDist ptDist(m_cfg.ptRange.first, m_cfg.ptRange.second);
  FW::UniformDist qDist(0., 1.);

  Acts::PerigeeSurface surface({0., 0., 0.});

  // loop over number of particles
  for (size_t ip = 0; ip < m_cfg.nparticles; ++ip) {
    /// get the d0 and z0
    double d0     = d0Dist(rng);
    double z0     = z0Dist(rng);
    double phi    = phiDist(rng);
    double eta    = etaDist(rng);
    double theta  = 2 * atan(exp(-eta));
    double pt     = ptDist(rng);
    double p      = pt / sin(theta);
    double charge = qDist(rng) > 0.5 ? 1. : -1.;
    double qop    = charge / p;
    // parameters
    Acts::ActsVectorD<5> pars;
    pars << d0, z0, phi, theta, qop;
    // some screen output
    std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
    // execute the test for charged particles
    if (charge) {
      // charged extrapolation - with hit recording
      Acts::BoundParameters startParameters(
          std::move(cov), std::move(pars), surface);
      if (executeTest<Acts::TrackParameters>(startParameters, cCells)
          != FW::ProcessCode::SUCCESS)
        ACTS_VERBOSE(
            "Test of charged parameter extrapolation did not succeed.");
    } else {
      // neutral extrapolation
      Acts::NeutralBoundParameters neutralParameters(
          std::move(cov), std::move(pars), surface);
      // prepare hits for charged neutral paramters - no hit recording
      if (executeTest<Acts::NeutralParameters>(neutralParameters, nCells)
          != FW::ProcessCode::SUCCESS)
        ACTS_WARNING(
            "Test of neutral parameter extrapolation did not succeed.");
    }
  }

  // - the extrapolation cells - charged - if configured
  if (m_cfg.simulatedChargedExCellCollection != ""
      && ctx.eventStore.add(m_cfg.simulatedChargedExCellCollection,
                            std::move(cCells))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // - the extrapolation cells - neutral - if configured
  if (m_cfg.simulatedNeutralExCellCollection != ""
      && ctx.eventStore.add(m_cfg.simulatedNeutralExCellCollection,
                            std::move(nCells))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  // return process code
  return FW::ProcessCode::SUCCESS;
}
