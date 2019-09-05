// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <memory>

#include <boost/program_options.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/GenericDetector/GenericDetector.hpp"
#include "ACTFW/Geometry/CommonGeometry.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvReaderOptions.hpp"
#include "ACTFW/Utilities/Options.hpp"

int
main(int argc, char* argv[])
{
  // setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addGeometryOptions(desc);
  FW::Options::addCsvReaderOptions(desc);
  FW::Options::addOutputOptions(desc);

  GenericDetector detector;
  detector.addOptions(desc);

  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) { return EXIT_FAILURE; }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // Now read the standard options
  auto logLevel = FW::Options::readLogLevel(vm);
  auto nEvents  = FW::Options::readSequencerConfig(vm).events;

  // setup detector geometry
  auto geometry         = FW::Geometry::build(vm, detector);
  auto trackingGeometry = geometry.first;
  // Add context decorators
  for (auto cdr : geometry.second) { sequencer.addContextDecorator(cdr); }

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
    plCsvReaderConfig.tGeometry               = trackingGeometry;
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
