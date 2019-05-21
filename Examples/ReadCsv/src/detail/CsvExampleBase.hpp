// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/GeometryOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvReaderOptions.hpp"
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

/// @brief The ReadCsv example
///
/// @tparam options_setup_t are the callable example options
/// @tparam geometry_setup_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param argv the argument list
/// @param optionsSetup is a callable options struct
/// @param geometrySetup is a callable geometry getter
template <typename options_setup_t, typename geometry_setup_t>
int
CsvExample(int               argc,
           char*             argv[],
           options_setup_t&  optionsSetup,
           geometry_setup_t& geometrySetup)

{
  // setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addGeometryOptions(desc);
  FW::Options::addCsvReaderOptions(desc);
  FW::Options::addOutputOptions(desc);

  optionsSetup(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // Now read the standard options
  auto logLevel          = FW::Options::readLogLevel(vm);
  auto nEvents           = FW::Options::readSequencerConfig(vm).events;
  auto geometry          = geometrySetup(vm);
  auto tGeometry         = geometry.first;
  auto contextDecorators = geometry.second;

  // Add it to the sequencer
  for (auto cdr : contextDecorators) {
    sequencer.addContextDecorator(cdr);
  }

  // Input directory
  std::string inputDir = vm["input-dir"].as<std::string>();

  // Input filenames
  std::string inputParticlesFile = vm["input-particle-file"].as<std::string>();
  std::string inputHitsFileName  = vm["input-hit-file"].as<std::string>();
  std::string inputDetailsFileName = vm["input-detail-file"].as<std::string>();
  std::string inputTruthFileName   = vm["input-truth-file"].as<std::string>();

  // Output collection name
  std::string outputParticleCollection
      = vm["output-particle-collection"].as<std::string>();
  std::string outputClusterCollection
      = vm["output-plCluster-collection"].as<std::string>();

  // Read particles as CSV files
  if (vm["read-particle-csv"].as<bool>()) {
    FW::Csv::CsvParticleReader::Config particleCsvReaderConfig;
    particleCsvReaderConfig.inputDir      = inputDir;
    particleCsvReaderConfig.inputFileName = inputParticlesFile + ".csv";
    particleCsvReaderConfig.outputParticleCollection = outputParticleCollection;
    sequencer.addReader(std::make_shared<FW::Csv::CsvParticleReader>(
        particleCsvReaderConfig, logLevel));
  }

  // Read clusters as CSV files
  if (vm["read-plCluster-csv"].as<bool>()) {
    FW::Csv::CsvPlanarClusterReader::Config plCsvReaderConfig;
    plCsvReaderConfig.tGeometry               = tGeometry;
    plCsvReaderConfig.inputDir                = inputDir;
    plCsvReaderConfig.inputHitsFileName       = inputHitsFileName + ".csv";
    plCsvReaderConfig.inputDetailsFileName    = inputDetailsFileName + ".csv";
    plCsvReaderConfig.inputTruthFileName      = inputTruthFileName + ".csv";
    plCsvReaderConfig.outputClusterCollection = outputClusterCollection;
    sequencer.addReader(std::make_shared<FW::Csv::CsvPlanarClusterReader>(
        plCsvReaderConfig, logLevel));
  }

  return sequencer.run();
}
