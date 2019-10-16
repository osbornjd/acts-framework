// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Fitting/FittingAlgorithm.hpp"

#include "ACTFW/Utilities/OptionsFwd.hpp"

#include <iostream>

namespace FW {

namespace Options {

  /// @brief read the Fitting options
  ///
  /// Adding Fitting specific options to the Options package
  ///
  /// @param [in] opt_t The options object where the specific digitization
  /// options are attached to
  void
  addFittingOptions(boost::program_options::options_description& opt);

  /// @brief read the fitter specific options and return a Config file
  ///
  ///@param vm the options map to be read out
  void
  readFittingConfig(const boost::program_options::variables_map& vm,
                    FittingAlgorithm::Config&                    fittingConfig);

}  // namespace Options
}  // namespace FW
