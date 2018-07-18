// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// CommonOptions.hpp
///////////////////////////////////////////////////////////////////

#pragma once

#include <utility>
#include "Acts/Utilities/Logger.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  /// @brief add collections options
  ///
  /// @tparam Type of the boost option object
  ///
  /// @param defaultEvent is the number of event to be executed
  /// @param defaultValue is the log level default value
  template <typename aopt_t>
  void
  addCollectionsOptions(aopt_t& opt)
  {
    opt.add_options()("evgen-particles",
                      po::value<std::string>()->default_value("EvgenParticles"),
                      "The name of the generated particles.");
  }

  /// Read evgen collection
  ///
  /// @tparam amap_t Type of the options map
  ///
  /// @param[in] vm Map to be read in
  template <typename amap_t>
  std::string
  readEvgenCollectionName(const amap_t& vm)
  {
    return vm["evgen-particles"].template as<std::string>();
  }

}  // end of namespace Options
}  // end of namespace FW
