// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Fatras/FatrasAlgorithm.hpp"

template <class MaterialEngine>
FW::FatrasAlgorithm<MaterialEngine>::FatrasAlgorithm(
    const Config&        cfg,
    Acts::Logging::Level loglevel)
  : FW::BareAlgorithm("FatrasAlgorithm", loglevel)
  , m_cfg(cfg)
  , m_exAlgorithm(
        std::make_unique<const FW::ExtrapolationAlgorithm>(m_cfg.exConfig,
                                                           loglevel))
{
}

template <class MaterialEngine>
FW::ProcessCode
FW::FatrasAlgorithm<MaterialEngine>::execute(
    const FW::AlgorithmContext context) const
{
  if (m_cfg.materialInteractionEngine){
    // Create an algorithm local random number generator
    FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(context);
    // Set the random generator of the material interaction
    m_cfg.materialInteractionEngine->setRandomGenerator(rng);
  }
  // call the extrapolation algorithm
  m_exAlgorithm->execute(context);

  return FW::ProcessCode::SUCCESS;
}
