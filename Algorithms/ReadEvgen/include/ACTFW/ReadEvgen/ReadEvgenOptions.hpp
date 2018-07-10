// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addEvgenReaderOptions(AOPT& opt)
  {
    opt.add_options()("evg-collection",
                      po::value<std::string>()->default_value("EvgenParticles"),
                      "Collection name of the evgen particles.")(
        "evg-shuffle",
        po::value<bool>()->default_value(false),
        "Shuffle the order of events after reading.")(
        "evg-pileup",
        po::value<int>()->default_value(200),
        "Number of instantaneous pile-up events.")(
        "evg-vertex-xy-range",
        po::value<read_range>()->multitoken()->default_value({0., 0.015}),
        "transverse range of the vertex in xy. Please hand over by simply "
        "seperating the values by space")(
        "evg-vertex-z-range",
        po::value<read_range>()->multitoken()->default_value({0., 5.5}),
        "transverse range of the vertex in z.  Please hand over by simply "
        "seperating the values by space");
  }

  /// read the evgen options and return a Config file
  template <class AMAP>
  FW::EvgenReader::Config
  readEvgenConfig(const AMAP& vm)
  {

    FW::EvgenReader::Config readEvgenConfig;
    readEvgenConfig.evgenCollection
        = vm["evg-collection"].template as<std::string>();
    readEvgenConfig.pileupPoissonParameter
        = vm["evg-pileup"].template as<int>();
    // vertex parameters
    auto vtpars = vm["evg-vertex-xy-range"].template as<read_range>();
    auto vzpars = vm["evg-vertex-z-range"].template as<read_range>();
    readEvgenConfig.vertexTParameters = {{vtpars[0], vtpars[1]}};
    readEvgenConfig.vertexZParameters = {{vzpars[0], vzpars[1]}};
    // shuffle the event or not
    readEvgenConfig.shuffleEvents = vm["evg-shuffle"].template as<bool>();
    // return the config
    return readEvgenConfig;
  }
}
}
