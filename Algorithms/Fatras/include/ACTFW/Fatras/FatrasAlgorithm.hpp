// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>
#include <limits>
#include <memory>

#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

namespace FW {

/// Fast track simulation using the Acts propagation and navigation.
///
/// @tparam simulator_t the Fatras simulation kernel type
template <typename simulator_t>
class FatrasAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// The simulation kernel
    simulator_t simulator;

    /// FW random number service
    std::shared_ptr<FW::RandomNumbers> randomNumberSvc = nullptr;

    /// the input event collection name
    std::string inputEventCollection;

    /// the simulated particles output collection name
    std::string simulatedEventCollection;

    /// the simulated hit output collection name
    std::string simulatedHitCollection;

    /// @brief Config constructor with propagator type
    ///
    /// @param s the Fatras simulation kernel
    Config(simulator_t&& s) : simulator(std::move(s)) {}
  };

  /// Construct the algorithm from a config.
  ///
  /// @param cfg is the configuration struct
  /// @param lvl is the logging level
  FatrasAlgorithm(const Config& cfg, Acts::Logging::Level lvl);

  /// Run the simulation for a single event.
  ///
  /// @param ctx the algorithm context containing all event information
  FW::ProcessCode
  execute(const AlgorithmContext& ctx) const final override;

private:
  Config m_cfg;
};

}  // namespace FW

#include "FatrasAlgorithm.ipp"
