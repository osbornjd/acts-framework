// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "FatrasAlgorithm.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <typename AOPT>
  void
  addFatrasOptions(AOPT& opt)
  {
    opt.add_options()(
        "fatras-sim-particles",
        po::value<std::string>()->default_value("FatrasParticles"),
        "The collection of simulated particles.")(
        "fatras-sim-hits",
        po::value<std::string>()->default_value("FatrasHits"),
        "The collection of simulated hits");
  }

  /// read the evgen options and return a Config file
  template <typename AMAP, typename simulator_t>
  typename FatrasAlgorithm<simulator_t>::Config
  readFatrasConfig(const AMAP& vm, simulator_t propagator)
  {
    // create a config
    typename FatrasAlgorithm<simulator_t>::Config fatrasConfig(
        std::move(propagator));
    // set the collections
    fatrasConfig.simulatedHitCollection
        = vm["fatras-sim-hits"].template as<std::string>();
    fatrasConfig.simulatedParticleCollection
        = vm["fatras-sim-particles"].template as<std::string>();
    // and return the config
    return fatrasConfig;
  }
}
}
