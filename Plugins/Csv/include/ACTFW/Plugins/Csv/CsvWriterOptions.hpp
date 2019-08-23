// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>

#include "ACTFW/Plugins/Csv/CsvTrackingGeometryWriter.hpp"

namespace FW {
namespace Options {

  // Add common CSV writer options.
  void
  addCsvWriterOptions(boost::program_options::options_description& opt);

  /// Read the CSV tracking geometry writer options.
  FW::Csv::CsvTrackingGeometryWriter::Config
  readCsvTrackingGeometryWriterConfig(
      const boost::program_options::variables_map& vm);

}  // namespace Options
}  // namespace FW
