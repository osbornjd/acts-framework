// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

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
///
/// @tparam simulator_t The Fatras simulation kernel type
/// @tparam event_collection_t  The event collection type
/// @tparam particle_collection_t The Paritlce type
/// @tparam hit_t The hit Type
namespace FW {

template <typename simulator_t, typename event_collection_t, typename hit_t>
class FatrasAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {

    /// @brief Config constructor with propagator type
    ///
    /// @param fsimulator Propagator object for charged particles
    Config(simulator_t fsimulator) : simulator(std::move(fsimulator)) {}

    /// The simulation kernel
    simulator_t simulator;

    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc = nullptr;

    /// the input event collection name
    std::string inputEventCollection = "";

    /// the simulated particles output collection name
    std::string simulatedEventCollection = "";

    /// the simulated hit output collection name
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
