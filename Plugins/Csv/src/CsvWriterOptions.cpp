// This file is part of the Acts project.
//
// Copyright (C) 2010 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvWriterOptions.hpp"

void
FW::Options::addCsvWriterOptions(
    boost::program_options::options_description& opt)
{
  using namespace boost::program_options;

  opt.add_options()("csv-output-precision",
                    value<size_t>()->default_value(6),
                    "Floating number output precision.")(
      "csv-tg-perevent", bool_switch(), "Write tracking geometry per event.");
}

FW::Csv::CsvParticleWriter::Config
FW::Options::readCsvParticleWriter(
    const boost::program_options::variables_map& vm)
{
  FW::Csv::CsvParticleWriter::Config cfg;
  if (not vm["output-dir"].empty()) {
    cfg.outputDir = vm["output-dir"].as<std::string>();
  }
  cfg.outputPrecision = vm["csv-output-precision"].as<size_t>();
  return cfg;
}

FW::Csv::CsvPlanarClusterWriter::Config
FW::Options::readCsvPlanarClusterWriterConfig(
    const boost::program_options::variables_map& vm)
{
  FW::Csv::CsvPlanarClusterWriter::Config cfg;
  if (not vm["output-dir"].empty()) {
    cfg.outputDir = vm["output-dir"].as<std::string>();
  }
  cfg.outputPrecision = vm["csv-output-precision"].as<size_t>();
  return cfg;
}

FW::Csv::CsvTrackingGeometryWriter::Config
FW::Options::readCsvTrackingGeometryWriterConfig(
    const boost::program_options::variables_map& vm)
{
  FW::Csv::CsvTrackingGeometryWriter::Config cfg;
  if (not vm["output-dir"].empty()) {
    cfg.outputDir = vm["output-dir"].as<std::string>();
  }
  cfg.outputPrecision = vm["csv-output-precision"].as<size_t>();
  cfg.writePerEvent   = vm.count("csv-tg-perevent");
  return cfg;
}
