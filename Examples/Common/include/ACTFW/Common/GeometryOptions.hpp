// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdlib>
#include <iostream>
#include <utility>
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {
namespace Options {

  /// @brief common geometry / material options
  ///
  /// This helps to define options the for the generic detector building
  /// It allows to set/steer material proxies and binning
  ///
  /// @tparam aopt_t Type of the options map (from boost program options)
  /// @param opt The options object to which these options are attached
  template <typename aopt_t>
  void
  addGeometryOptions(aopt_t& opt)
  {
    opt.add_options()("geo-surface-loglevel",
                      po::value<size_t>()->default_value(3),
                      "The outoput log level for the surface building.")(
        "geo-layer-loglevel",
        po::value<size_t>()->default_value(3),
        "The output log level for the layer building.")(
        "geo-volume-loglevel",
        po::value<size_t>()->default_value(3),
        "The output log level for the volume building.")(
        "geo-subdetectors",
        po::value<read_strings>()->multitoken()->default_value({{}}),
        "Sub detectors for the output writing")(
        "geo-material-mode",
        po::value<size_t>()->default_value(1),
        "Modes are: None (0) , Constructed (1), Loaded (2), Proxy (3)");
  }
}  // namespace Options
}  // namespace FW
