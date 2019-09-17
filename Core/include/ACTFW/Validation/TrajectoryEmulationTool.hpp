// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <functional>
#include <map>
#include <string>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimSourceLink.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {

// Tools to emulate outliers and holes on truth trajectory
class TrajectoryEmulationTool
{
  using Identifier            = Data::SimSourceLink;
  using SourceLinks           = std::vector<Data::SimSourceLink>;
  using SourceLinksVector     = std::vector<std::vector<Data::SimSourceLink>>;
  using MultiplicityGenerator = std::function<size_t(RandomEngine&)>;

public:
  /// @brief The nested configuration struct
  struct Config
  {
    MultiplicityGenerator trajectoryMultiplicity
        = nullptr;  ///< generator for number of emulated trajectory
    MultiplicityGenerator outlierMultiplicity
        = nullptr;  ///< generator for number of outliers on emulated track
    MultiplicityGenerator holeMultiplicity
        = nullptr;  ///< generator for number of holes on emulated track
    std::shared_ptr<RandomNumbers> randomNumbers
        = nullptr;  ///< random number service
  };

  /// Constructor
  ///
  /// @param cfg Configuration struct
  /// @param level Message level declaration
  TrajectoryEmulationTool(const Config&        cfg,
                          Acts::Logging::Level level = Acts::Logging::INFO);

  /// @brief emulate trajectories with outliers given a truth trajectory
  /// @param truthSourceLinks is the truth trajectory
  /// @return a vector of emulated trajectory
  SourceLinksVector
  emulate(const AlgorithmContext& ctx,
          const SourceLinks&      truthSourceLinks) const;

private:
  Config                              m_cfg;     ///< The Config class
  std::unique_ptr<const Acts::Logger> m_logger;  ///< The logging instance

  /// The logger
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW
