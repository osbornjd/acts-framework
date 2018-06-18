// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
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

namespace au = Acts::units;

namespace FW {

namespace Options {

  /// the particle gun options, the are prefixes with gp
  template <class AOPT>
  void
  addGeometryOptions(AOPT& opt)
  {
    opt.add_options()("geo-surfaceloglevel",
                      po::value<size_t>()->default_value(3),
                      "The outoput log level for the surface building.")
                      ("geo-layerloglevel",
                      po::value<size_t>()->default_value(3),
                      "The output log level for the layer building.")
                      ("geo-volumeloglevel",
                      po::value<size_t>()->default_value(3),
                      "The output log level for the volume building.");
  }
} // namespace Options
} // namespace FW
