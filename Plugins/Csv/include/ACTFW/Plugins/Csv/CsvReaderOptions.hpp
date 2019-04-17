// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // Common CSV writer option
  ///
  /// @tparam aopt_t Type of the options object (from BOOST)
  ///
  /// @param opt The options object, where string based options are attached
  template <typename aopt_t>
  void
  addCsvReaderOptions(aopt_t& opt)
  {
    opt.add_options()("read-particle-csv",
                      po::value<bool>()->default_value(false),
                      "Switch on to read '.csv' file as particles.")(
        "read-plCluster-csv",
        po::value<bool>()->default_value("false"),
        "Switch on to read '.csv' file as planarClusters.")(
        "input-dir",
        po::value<std::string>()->default_value(""),
        "Input directory location.")(
        "input-particle-file",
        po::value<std::string>()->default_value("evgen-particles"),
        "Name of '.csv' input file with particles info.")(
        "input-hit-file",
        po::value<std::string>()->default_value("hits"),
        "Name of '.csv' input file with hits info.")(
        "input-detail-file",
        po::value<std::string>()->default_value("details"),
        "Name of '.csv' input file with hit details info.")(
        "input-truth-file",
        po::value<std::string>()->default_value("truth"),
        "Name of '.csv' input file with truth info.")(
        "output-particle-collection",
        po::value<std::string>()->default_value("SimParticle"),
        "Name of output particles collection.")(
        "output-plCluster-collection",
        po::value<std::string>()->default_value("PlanarCluster"),
        "Name of output planar clusters collection.");
  }

}  // namespace Options
}  // namespace FW
