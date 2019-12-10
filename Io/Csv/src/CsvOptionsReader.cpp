// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvOptionsReader.hpp"

#include <boost/program_options.hpp>

FW::Csv::CsvParticleReader::Config
FW::Options::readCsvParticleReaderConfig(const Variables& vm)
{
  FW::Csv::CsvParticleReader::Config cfg;
  if (not vm["input-dir"].empty()) {
    cfg.inputDir = vm["input-dir"].as<std::string>();
  }
  return cfg;
}

FW::Csv::CsvPlanarClusterReader::Config
FW::Options::readCsvPlanarClusterReaderConfig(const Variables& vm)
{
  FW::Csv::CsvPlanarClusterReader::Config cfg;
  if (not vm["input-dir"].empty()) {
    cfg.inputDir = vm["input-dir"].as<std::string>();
  }
  return cfg;
}
