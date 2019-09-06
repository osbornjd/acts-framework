// This file is part of the Acts project.
//
// Copyright (C) 2010 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvWriterOptions.hpp"

#include <boost/program_options.hpp>

void
FW::Options::addCsvWriterOptions(FW::Options::Description& desc)
{
  using namespace boost::program_options;

  desc.add_options()("csv-output-precision",
                     value<size_t>()->default_value(6),
                     "Floating number output precision.")(
      "csv-tg-perevent", bool_switch(), "Write tracking geometry per event.");
}

FW::Csv::CsvParticleWriter::Config
FW::Options::readCsvParticleWriterConfig(const FW::Options::Variables& vm)
{
  FW::Csv::CsvParticleWriter::Config cfg;
  if (not vm["output-dir"].empty()) {
    cfg.outputDir = vm["output-dir"].as<std::string>();
  }
  cfg.outputPrecision = vm["csv-output-precision"].as<size_t>();
  return cfg;
}

FW::Csv::CsvPlanarClusterWriter::Config
FW::Options::readCsvPlanarClusterWriterConfig(const FW::Options::Variables& vm)
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
    const FW::Options::Variables& vm)
{
  FW::Csv::CsvTrackingGeometryWriter::Config cfg;
  if (not vm["output-dir"].empty()) {
    cfg.outputDir = vm["output-dir"].as<std::string>();
  }
  cfg.outputPrecision = vm["csv-output-precision"].as<size_t>();
  cfg.writePerEvent   = vm.count("csv-tg-perevent");
  return cfg;
}
