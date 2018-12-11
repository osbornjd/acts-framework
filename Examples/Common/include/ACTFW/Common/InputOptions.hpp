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

  /// @brief the common output options that are added to the
  /// job options
  ///
  /// @tparam aopt_t Type of the options object, bound to boost API
  /// @param [in] opt The options object for attaching specific options
  template <typename aopt_t>
  void
  addInputOptions(aopt_t& opt)
  {
    // Add specific options for this example
    opt.add_options()("input-dir",
                      po::value<std::string>()->default_value(""),
                      "Input directory location.")(
        "input-root",
        po::value<bool>()->default_value(false),
        "Switch on to write '.root' output file(s).")(
        "input-files",
        po::value<std::string>()->default_value(""),
        "The input file list, comma separated if needed.")(
        "input-collection",
        po::value<std::string>()->default_value(""),
        "The input collection to be read.");
  }
}  // namespace Options
}  // namespace FW