// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Plugins/Txt/TxtSurfaceWriter.hpp"
#include "ACTFW/Plugins/Txt/TxtTrackingGeometryWriter.hpp"
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // Common Txt writer option
  ///
  /// @tparam aopt_t Type of the options object (from BOOST)
  ///
  /// @param opt The options object, where string based options are attached
  template <typename aopt_t>
  void
  addTxtWriterOptions(aopt_t& opt)
  {
    opt.add_options()("txt-hit-outputPrecission",
                      po::value<int>()->default_value(6),
                      "Floating number output precission.");
  }

}  // namespace Options
}  // namespace FW
