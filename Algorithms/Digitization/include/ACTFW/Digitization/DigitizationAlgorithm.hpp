// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <string>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"

namespace Acts {
class PlanarModuleStepper;
class TrackingGeomtetyr;
}  // namespace Acts

namespace FW {

/// Create planar clusters from simulation hits.
class DigitizationAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// Input collection of simulated hits.
    std::string inputSimulatedHits;
    /// Output collection of clusters.
    std::string outputClusters;
    /// Module stepper for geometric clustering.
    std::shared_ptr<Acts::PlanarModuleStepper> planarModuleStepper = nullptr;
    /// Random numbers tool.
    std::shared_ptr<RandomNumbers> randomNumbers = nullptr;
  };

  /// Construct the digitization algorithm.
  ///
  /// @param cfg is the algorithm configuration
  /// @param lvl is the logging level
  DigitizationAlgorithm(Config cfg, Acts::Logging::Level lvl);

  /// Build clusters from input simulation hits.
  ///
  /// @param txt is the algorithm context with event information
  /// @return a process code indication success or failure
  ProcessCode
  execute(const AlgorithmContext& ctx) const final override;

private:
  Config m_cfg;
};

}  // namespace FW
