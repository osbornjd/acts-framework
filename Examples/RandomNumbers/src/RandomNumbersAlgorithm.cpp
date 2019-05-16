// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "RandomNumbersAlgorithm.hpp"

#include <iostream>
#include <random>

#include "ACTFW/Random/RandomNumbersSvc.hpp"

FW::RandomNumbersAlgorithm::RandomNumbersAlgorithm(
    const RandomNumbersAlgorithm::Config& cfg,
    Acts::Logging::Level                  level)
  : BareAlgorithm("RandomNumbersAlgorithm", level), m_cfg(cfg)
{
  if (!m_cfg.randomNumbers) {
    throw std::invalid_argument("Missing random number service");
  }
}

FW::ProcessCode
FW::RandomNumbersAlgorithm::execute(const AlgorithmContext& context) const
{

  ACTS_INFO("Running random number generation");
  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(context);

  // Spawn some random number distributions
  std::normal_distribution<double> gaussDist(m_cfg.gaussParameters[0],
                                             m_cfg.gaussParameters[1]);
  std::uniform_real_distribution<double> uniformDist(
      m_cfg.uniformParameters[0], m_cfg.uniformParameters[1]);
  std::gamma_distribution<double> gammaDist(m_cfg.gammaParameters[0],
                                            m_cfg.gammaParameters[1]);
  std::poisson_distribution<int> poissonDist(m_cfg.poissonParameter);

  ACTS_INFO(m_cfg.drawsPerEvent << " draws per event will be done");

  for (size_t idraw = 0; idraw < m_cfg.drawsPerEvent; ++idraw) {
    double gauss   = gaussDist(rng);
    double uniform = uniformDist(rng);
    double gamma   = gammaDist(rng);
    int    poisson = poissonDist(rng);

    ACTS_VERBOSE("Gauss   : " << gauss);
    ACTS_VERBOSE("Uniform : " << uniform);
    ACTS_VERBOSE("Gamma   : " << gamma);
    ACTS_VERBOSE("Poisson : " << poisson);
  }
  return FW::ProcessCode::SUCCESS;
}
