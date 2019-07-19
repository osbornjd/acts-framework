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
#include "EventToTrackConverter.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  /// @brief EventToTrackConverter options
  /// Adds specific EventToTrackConverter options to the boost::program_options
  ///
  /// @tparam aopt_t Type of the options object (API bound to boost)
  ///
  /// @param [in] opt_t The options object where the specific
  /// EventToTrackConverter
  /// options are attached to
  template <typename aopt_t>
  void
  addEventToTrackConverterOptions(aopt_t& opt)
  {
    opt.add_options()("input-name",
                      po::value<std::string>()->default_value("input_event"),
                      "Track converter input collection name.")(
        "output-name",
        po::value<std::string>()->default_value("track_output"),
        "Collection name of the produced tracks.");
  }

  ///@brief  Read the EventToTrackConverter options and return a Config object
  ///
  ///@tparam omap_t Type of the options map
  ///@param vm the options map to be read out
  template <typename omap_t>
  EventToTrackConverterAlgorithm::Config
  readEventToTrackConverterConfig(const omap_t& vm)
  {
    // create a config
    EventToTrackConverterAlgorithm::Config convConfig;
    convConfig.inputCollection  = vm["input-name"].template as<std::string>();
    convConfig.outputCollection = vm["output-name"].template as<std::string>();
    // and return the config
    return convConfig;
  }
}  // namespace Options
}  // namespace FW
