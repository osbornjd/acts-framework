// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_OPTIONS_DIGITIZATIONOPTIONS_HPP
#define ACTFW_OPTIONS_DIGITIZATIONOPTIONS_HPP

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Units.hpp"
#include "DigitizationAlgorithm.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common digitization options
  template <typename AOPT>
  void
  addDigitizationOptions(AOPT& opt)
  {
    opt.add_options()(
        "digi-spacepoints",
        po::value<std::string>()->default_value("FatrasSpacePoints"),
        "Collection name of the produced space points.")(
        "digi-clusters",
        po::value<std::string>()->default_value("FatrasClusters"),
        "Collection name of the produced clustes.")(
        "digi-resolution-file",
        po::value<std::string>()->default_value(""),
        "Name of the resolution file (root format).");
  }

  /// read the digitization options and return a Config object
  template <typename AMAP>
  DigitizationAlgorithm::Config
  readDigitizationConfig(const AMAP& vm)
  {
    // create a config
    DigitizationAlgorithm::Config digiConfig;
    digiConfig.spacePointCollection
        = vm["digi-spacepoints"].template as<std::string>();
    digiConfig.clusterCollection
        = vm["digi-clusters"].template as<std::string>();
    digiConfig.resolutionFile
        = vm["digi-resolution-file"].template as<std::string>();
    // and return the config
    return digiConfig;
  }
}  // namespace Options
}  // namespace FW

#endif  // ACTFW_OPTIONS_DIGITIZATIONOPTIONS_HPP
