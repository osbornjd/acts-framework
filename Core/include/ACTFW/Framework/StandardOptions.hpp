// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// Options.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_FRAMEWORK_OPTIONS_HPP
#define ACTFW_FRAMEWORK_OPTIONS_HPP

#include <utility>
#include "Acts/Utilities/Logger.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // add standard options
  template <class AOPT>
  void
  addStandardOptions(AOPT& opt, size_t defaultEvents, size_t defaultValue)
  {
    opt.add_options()("help", "Produce help message")(
        "events,n",
        po::value<size_t>()->default_value(defaultEvents),
        "The number of events to be processed")(
        "loglevel,l",
        po::value<size_t>()->default_value(defaultValue),
        "The output log level. Please set the wished number (0 = VERBOSE, 1 = "
        "DEBUG, 2 = INFO, 3 = WARNING, 4 = ERROR, 5 = FATAL).");
  }

  // read standard options
  template <class AMAP>
  std::pair<size_t, Acts::Logging::Level>
  readStandardOptions(const AMAP& vm)
  {

    size_t nEvents = vm["events"].template as<size_t>();
    std::cout << "- running " << nEvents << " events " << std::endl;
    Acts::Logging::Level logLevel
        = Acts::Logging::Level(vm["loglevel"].template as<size_t>());
    if (vm.count("loglevel")) {
      logLevel = Acts::Logging::Level(vm["loglevel"].template as<size_t>());
      std::cout << "- the output log level is set to " << logLevel << std::endl;
    } else {
      std::cout << "- default log level is " << logLevel << std::endl;
    }
    return std::pair<size_t, Acts::Logging::Level>(nEvents, logLevel);
  }
}
}

#endif  //
