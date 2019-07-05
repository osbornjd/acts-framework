// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Units.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  /// @brief Material mapping options, specially added
  ///
  /// @tparam aopt_t Type of the options object (API bound to boost)
  ///
  /// @param [in] opt_t The options object where the specific digitization
  /// options are attached to
  template <typename aopt_t>
  void
  addMaterialMappingOptions(aopt_t& opt)
  {
    opt.add_options()(
        "mat-mapping-collection",
        po::value<std::string>()->default_value("material-tracks"),
        "Collection name of the material tracks for reading.");
  }

  ///@brief  Read the digitization options and return a Config object
  ///
  ///@tparam omap_t Type of the options map
  ///@param vm the options map to be read out
  // template <typename omap_t>
  // DigitizationAlgorithm::Config
  // readDigitizationConfig(const omap_t& vm)
  //{
  //  // create a config
  //  DigitizationAlgorithm::Config digiConfig;
  //  digiConfig.spacePointCollection
  //      = vm["digi-spacepoints"].template as<std::string>();
  //  digiConfig.clusterCollection
  //      = vm["digi-clusters"].template as<std::string>();
  //  digiConfig.resolutionFile
  //      = vm["digi-resolution-file"].template as<std::string>();
  //  // and return the config
  //  return digiConfig;
  //}
}  // namespace Options
}  // namespace FW