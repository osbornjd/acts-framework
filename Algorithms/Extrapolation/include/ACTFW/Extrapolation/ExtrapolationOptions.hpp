// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_OPTIONS_EXTRAPOLATIONOPTIONS_HPP
#define ACTFW_OPTIONS_EXTRAPOLATIONOPTIONS_HPP

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addExtrapolationOptions(AOPT& opt)
  {
    opt.add_options()("ext-fatras",
                      po::value<bool>()->default_value(true),
                      "Use the fatras option.")
                      ("ext-energyloss",
                      po::value<bool>()->default_value(true),
                      "Apply energy loss correction.")
                     ("ext-scattering",
                      po::value<bool>()->default_value(true),
                      "Apply scattering correction.")
                     ("ext-hadronic",
                      po::value<bool>()->default_value(true),
                      "Apply hadronic interaction (Fatras only).");
  }

  /// read the evgen options and return a Config file
  template <class AMAP>
  std::array<bool, 4>
  readExtrapolationOptions(const AMAP& vm)
  {
    bool fatras     = vm["ext-fatras"].template as<bool>();
    bool eloss      = vm["ext-energyloss"].template as<bool>();
    bool scattering = vm["ext-scattering"].template as<bool>();
    bool hadronic   = vm["ext-hadronic"].template as<bool>();
    // return the config
    return { fatras, eloss, scattering, hadronic };
  }
}

}

#endif // ACTFW_OPTIONS_EXTRAPOLATIONOPTIONS_HPP
