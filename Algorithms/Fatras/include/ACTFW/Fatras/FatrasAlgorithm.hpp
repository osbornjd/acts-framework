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
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

/// @class FatrasAlgorithm
///
/// The fatras algorithm runs the fast track simulation using the
/// Extrapolator from the acts-core toolkit.
///
/// Random numbers are reset per event using the event context

namespace FW {

template <typename propagator_t>
class FatrasAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {

    /// @brief Config constructor with propagator type
    ///
    /// @tparam is the propagator type
    /// @param pgt is the propagator object, not it is being moved
    Config(propagator_t pgt) : propagator(std::move(pgt)) {}

    /// The templated propagator
    propagator_t propagator;

    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;

    /// the particles input collection name
    std::string inputParticleCollection = "";

    /// the simulated particles output collection name
    std::string simulatedParticleCollection = "";

    /// the simulated particles output collection name
    std::string simulatedHitCollection = "";
  };

  /// Constructor
  /// @param [in] cnf is the configuration struct
  /// @param [in] loglevel is the logging level
  FatrasAlgorithm(const Config& cnf, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  /// @return a process code
  FW::ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  /// The config class
  Config m_cfg;
};

}  // namespace FW

#include "FatrasAlgorithm.ipp"

#endif  // ACTFW_ALGORITHMS_FATRASALGORITHM_H
