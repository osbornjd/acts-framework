// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"
#include "Acts/Utilities/Logger.hpp"

template <class T>
FW::ProcessCode
FW::ExtrapolationAlgorithm::executeTest(
    const T&                                 startParameters,
    std::vector<Acts::ExtrapolationCell<T>>& eCells) const
{
  // setup the extrapolation how you'd like it
  Acts::ExtrapolationCell<T> ecc(startParameters);
  // ecc.setParticleHypothesis(m_cfg.particleType);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::StopAtBoundary);
  ecc.addConfigurationMode(Acts::ExtrapolationMode::FATRAS);
  ecc.searchMode = m_cfg.searchMode;
  // now set the behavioral bits
  if (m_cfg.resolveSensitive)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectSensitive);
  if (m_cfg.resolvePassive)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectPassive);
  if (m_cfg.collectBoundary)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectBoundary);
  if (m_cfg.resolveMaterial)
    ecc.addConfigurationMode(Acts::ExtrapolationMode::CollectMaterial);
  if (m_cfg.sensitiveCurvilinear) ecc.sensitiveCurvilinear = true;

  // force a stop in the extrapoaltion mode
  if (m_cfg.pathLimit > 0.) {
    ecc.pathLimit = m_cfg.pathLimit;
    ecc.addConfigurationMode(Acts::ExtrapolationMode::StopWithPathLimit);
  }
  // screen output
  ACTS_DEBUG("===> forward extrapolation - collecting information <<===");

  // call the extrapolation engine
  Acts::ExtrapolationCode eCode = m_cfg.extrapolationEngine->extrapolate(ecc);
  if (eCode.isFailure()) {
    ACTS_WARNING("Extrapolation failed.");
    return FW::ProcessCode::ABORT;
  }

  /// fill the ecc step into the container at the end
  eCells.push_back(std::move(ecc));

  // return success
  return FW::ProcessCode::SUCCESS;
}
