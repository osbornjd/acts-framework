// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Generators/MultiplicityGenerators.hpp"
#include "ACTFW/Utilities/Options.hpp"
#include "TrajectoryEmulationTool.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  /// @brief read the Trajectory emulation tool options
  ///
  /// Adding Trajectory emulation specific options to the Options package
  ///
  /// @tparam aopt_t Type of the options object (API bound to boost)
  ///
  /// @param [in] opt_t The options object where the specific
  /// options are attached to
  template <typename aopt_t>
  void
  addTrajectoryEmulationOptions(aopt_t& opt)
  {
    opt.add_options()("emulation-gen-type",
                      po::value<std::string>()->default_value("poisson"),
                      "The generator for track emulation tool.")(
        "num-emulation-traj",
        po::value<int>()->default_value(1),
        "The number of emulated trajectories")(
        "num-emulation-hole",
        po::value<int>()->default_value(1),
        "The number of holes on emulated trajectory")(
        "num-emulation-outlier",
        po::value<int>()->default_value(1),
        "The number of outliers on emulated trajectory");
  }

  /// @brief read the emulation tool specific options and return a Config file
  ///
  ///@tparam omap_t Type of the options map
  ///@param vm the options map to be read out
  template <typename AMAP>
  typename TrajectoryEmulationTool::Config
  readTrajectoryEmulationConfig(const AMAP& vm)
  {
    // Create a config
    TrajectoryEmulationTool::Config trajEmulationToolConfig;

    // set the multiplicity generator for trajectory
    trajEmulationToolConfig.trajectoryMultiplicity = FixedMultiplicityGenerator{
        static_cast<size_t>(vm["num-emulation-traj"].template as<int>())};

    // set the multiplicity generator for holes and outliers
    auto multiplicityGenType
        = vm["emulation-gen-type"].template as<std::string>();
    if (multiplicityGenType == "poisson") {
      trajEmulationToolConfig.holeMultiplicity = PoissonMultiplicityGenerator{
          static_cast<size_t>(vm["num-emulation-hole"].template as<int>())};
      trajEmulationToolConfig.outlierMultiplicity
          = PoissonMultiplicityGenerator{static_cast<size_t>(
              vm["num-emulation-outlier"].template as<int>())};
    } else if (multiplicityGenType == "fixed") {
      trajEmulationToolConfig.holeMultiplicity = FixedMultiplicityGenerator{
          static_cast<size_t>(vm["num-emulation-hole"].template as<int>())};
      trajEmulationToolConfig.outlierMultiplicity = FixedMultiplicityGenerator{
          static_cast<size_t>(vm["num-emulation-outlier"].template as<int>())};
    } else {
      throw std::runtime_error("unknown generator input: " + multiplicityGenType
                               + " for trajectory emulation tool.");
    }

    // and return the config
    return std::move(trajEmulationToolConfig);
  }

}  // namespace Options
}  // namespace FW
