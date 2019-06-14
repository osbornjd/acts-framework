// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <Acts/Utilities/Logger.hpp>

namespace boost::program_options {
class options_description;
class variables_map;
}  // namespace boost::program_options

namespace FW {
namespace Options {

  /// Add common options needed e.g. for the sequencer.
  void
  addCommonOptions(boost::program_options::options_description& opt);

  /// Add common geometry-related options.
  void
  addGeometryOptions(boost::program_options::options_description& opt);

  /// Add common output-related options.
  void
  addOutputOptions(boost::program_options::options_description& opt);

  /// Read the log level.
  Acts::Logging::Level
  readLogLevel(const boost::program_options::variables_map& vm);

  /// Read number-of-events to be processed
  size_t
  readNumberOfEvents(const boost::program_options::variables_map& vm);

}  // namespace Options
}  // namespace FW
