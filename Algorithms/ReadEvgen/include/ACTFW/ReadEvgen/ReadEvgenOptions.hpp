// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Units.hpp"

namespace po = boost::program_options;

namespace au = Acts::units;

namespace FW {

namespace Options {

  /// Common evgen options, with an evg prefix
  ///
  /// @tparam aopt_t is the templated options object (from boost)
  template <typename aopt_t>
  void
  addEvgenReaderOptions(aopt_t& opt)
  {
    opt.add_options()(
        "evg-collection",
        po::value<std::string>()->default_value("evgen-particles"),
        "Collection name of the generated particles.")(
        "evg-shuffle",
        po::value<bool>()->default_value(false),
        "Shuffle the order of events after reading.")(
        "evg-pileup",
        po::value<int>()->default_value(200),
        "Number of instantaneous pile-up events.")(
        "evg-vertex-xy-range",
        po::value<read_range>()->multitoken()->default_value({0., 0.015}),
        "transverse range of the vertex in xy [mm]. Please hand over by simply "
        "seperating the values by space")(
        "evg-vertex-z-range",
        po::value<read_range>()->multitoken()->default_value({0., 55.5}),
        "transverse range of the vertex in z [mm].  Please hand over by simply "
        "seperating the values by space");
  }

  /// Read the evgen options and return a Config file
  ///
  /// @tparam vmap_t is the templated options map (from boost)
  template <typename vmap_t>
  FW::EvgenReader::Config
  readEvgenConfig(const vmap_t& vm)
  {
    FW::EvgenReader::Config readEvgenConfig;
    // the evgen Collection
    readEvgenConfig.evgenCollection
        = vm["evg-collection"].template as<std::string>();
    // the pileup Poisson parameter
    readEvgenConfig.pileupPoissonParameter
        = vm["evg-pileup"].template as<int>();
    // vertex parameters
    auto vtpars = vm["evg-vertex-xy-range"].template as<read_range>();
    auto vzpars = vm["evg-vertex-z-range"].template as<read_range>();
    readEvgenConfig.vertexTParameters
        = {{vtpars[0] * au::_mm, vtpars[1] * au::_mm}};
    readEvgenConfig.vertexZParameters
        = {{vzpars[0] * au::_mm, vzpars[1] * au::_mm}};
    // shuffle the event or not
    readEvgenConfig.shuffleEvents = vm["evg-shuffle"].template as<bool>();
    // return the config
    return readEvgenConfig;
  }
}
}
