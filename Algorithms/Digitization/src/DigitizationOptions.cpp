// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Digitization/DigitizationOptions.hpp"

#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace FW {

namespace Options {

  void
  addDigitizationOptions(boost::program_options::options_description& opt)
  {
    opt.add_options()("digi-spacepoints",
                      po::value<std::string>()->default_value("space-points"),
                      "Collection name of the produced space points.")(
        "digi-clusters",
        po::value<std::string>()->default_value("clusters"),
        "Collection name of the produced clustes.")(
        "digi-resolution-file",
        po::value<std::string>()->default_value(""),
        "Name of the resolution file (root format).");
  }

  DigitizationAlgorithm::Config
  readDigitizationConfig(const boost::program_options::variables_map& vm)
  {
    // create a config
    DigitizationAlgorithm::Config digiConfig;
    digiConfig.clusterCollection
        = vm["digi-clusters"].template as<std::string>();
    digiConfig.resolutionFile
        = vm["digi-resolution-file"].template as<std::string>();
    // and return the config
    return digiConfig;
  }
}  // namespace Options
}  // namespace FW
