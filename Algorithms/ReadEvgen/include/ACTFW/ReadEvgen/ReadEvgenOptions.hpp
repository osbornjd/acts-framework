// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// ReadEvgenOptions.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_EVGENOPTIONS_HPP
#define ACTFW_OPTIONS_EVGENOPTIONS_HPP

#include <iostream>
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addEvgenOptions(AOPT& opt)
  {
    opt.add_options()("evg-collection",
                      po::value<std::string>()->default_value("EvgenParticles"),
                      "Collection name of the evgen particles.")(
        "evg-pileup",
        po::value<int>()->default_value(200),
        "Number of instantaneous pile-up events.")(
        "evg-vertex-xyrange",
        po::value<read_range>()->multitoken()->default_value({0., 0.015}),
        "transverse range of the vertex in xy.")(
        "evg-vertex-zrange",
        po::value<read_range>()->multitoken()->default_value({0., 5.5}),
        "transverse range of the vertex in z.");
  }

  /// read the evgen options and return a Config file
  template <class AMAP>
  FW::ReadEvgenAlgorithm::Config
  readEvgenConfig(const AMAP& vm)
  {

    FW::ReadEvgenAlgorithm::Config readEvgenConfig;
    readEvgenConfig.evgenCollection
        = vm["evg-collection"].template as<std::string>();
    readEvgenConfig.pileupPoissonParameter
        = vm["evg-pileup"].template as<int>();
    // vertex parameters
    auto vtpars = vm["evg-vertex-xyrange"].template as<read_range>();
    auto vzpars = vm["evg-vertex-zrange"].template as<read_range>();
    readEvgenConfig.vertexTParameters = {{vtpars[0], vtpars[1]}};
    readEvgenConfig.vertexZParameters = {{vzpars[0], vzpars[1]}};
    // return the config
    return readEvgenConfig;
  }
}
}

#endif  // ACTFW_OPTIONS_EVGENOPTIONS_HPP