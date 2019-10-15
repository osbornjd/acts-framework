// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Utilities/OptionsFwd.hpp"

#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"

namespace FW {

namespace Options {

  /// @brief Digitization options
  /// Adds specific digitization options to the boost::program_options
  ///
  /// @param [in] opt_t The options object where the specific digitization
  /// options are attached to
  void
  addDigitizationOptions(boost::program_options::options_description& opt);

  ///@brief  Read the digitization options and return a Config object
  ///
  ///@tparam omap_t Type of the options map
  ///@param vm the options map to be read out
  readDigitizationConfig(const boost::program_options::variables_map& vm);
}  // namespace Options
}  // namespace FW
