// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_FATRASALGORITHM_H
#define ACTFW_ALGORITHMS_FATRASALGORITHM_H

#include <cmath>
#include <limits>
#include <memory>
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

namespace Acts {
class IExtrapolationEngine;
class TrackingGeometry;
class TrackingVolume;
}  // namespace Acts

/// @class FatrasAlgorithm
///
/// The fatras algorithm allows to hand over a custom MaterialEffectsEngine to
/// the extrapolation algorithm of which the random generator is set for every
/// event.

namespace FW {
/// @tparam MaterialEngine The material effects engine which should be used for
/// the simulation. It needs to have a function setRandomGenerator() in order to
/// set the random generator for every event.
template <class MaterialEngine>
class FatrasAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// The configuration of the internally used ExtrapolationAlgorithm
    ExtrapolationAlgorithm::Config exConfig;
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    /// The material interaction engine
    /// @note This engine must be the same engine used to create the
    /// extrapolation engine
    std::shared_ptr<MaterialEngine> materialInteractionEngine = nullptr;
  };

  /// Constructor
  /// @param [in] cnf is the configuration struct
  /// @param [in] loglevel is the loggin level
  FatrasAlgorithm(const Config& cnf, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  FW::ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  /// The config class
  Config m_cfg;
  /// The extrapolation algorithm
  std::unique_ptr<const FW::ExtrapolationAlgorithm> m_exAlgorithm;
};
}  // namespace FW
#include "FatrasAlgorithm.ipp"

#endif  // ACTFW_ALGORITHMS_FATRASALGORITHM_H
