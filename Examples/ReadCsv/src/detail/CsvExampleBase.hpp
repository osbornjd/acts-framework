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
#include "ACTFW/Plugins/Csv/CsvReaderOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"

namespace po = boost::program_options;

/// @brief The ReadCsv example
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param aegv the argument list
template <typename geometry_options_t, typename geometry_getter_t>
int
CsvExample(int                argc,
           char*              argv[],
           geometry_options_t geometryOptions,
           geometry_getter_t  trackingGeometry)
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // Now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Add the Common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add options for the Csv reading 
  FW::Options::addCsvReaderOptions<po::options_description>(desc);
  // Add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);
  // Add specific options for this geometry
  geometryOptions(desc);

  // Map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // Print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // Read the common options : number of events and log level
  auto nEvents  = FW::Options::readNumberOfEvents<po::variables_map>(vm);
  auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

  // Get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // Input directory
  std::string inputDir = vm["input-dir"].as<std::string>();

  // Input filenames 
  std::string inputParticlesFile = vm["input-particle-file"].as<std::string>();
  std::string inputHitsFileName = vm["input-hit-file"].as<std::string>();
  std::string inputDetailsFileName = vm["input-detail-file"].as<std::string>();
  std::string inputTruthFileName = vm["input-truth-file"].as<std::string>();

  // Output collection name 
  std::string outputParticleCollection = vm["output-particle-collection"].as<std::string>();
  std::string outputClusterCollection = vm["output-plCluster-collection"].as<std::string>();

  // Read particles as CSV files
  std::shared_ptr<FW::Csv::CsvParticleReader> particleCsvReader = nullptr;
  if (vm["read-particle-csv"].as<bool>()) {
    FW::Csv::CsvParticleReader::Config particleCsvReaderConfig;
    particleCsvReaderConfig.inputDir      = inputDir;
    particleCsvReaderConfig.inputFileName = inputParticlesFile + ".csv";
    particleCsvReaderConfig.outputParticleCollection = outputParticleCollection;
    particleCsvReader 
      = std::make_shared<FW::Csv::CsvParticleReader>(
                                                     particleCsvReaderConfig, logLevel);
  }

  // Read clusters as CSV files
  std::shared_ptr<FW::Csv::CsvPlanarClusterReader> plCsvReader = nullptr;
  if (vm["read-plCluster-csv"].as<bool>()) {
    FW::Csv::CsvPlanarClusterReader::Config plCsvReaderConfig;
    plCsvReaderConfig.tGeometry = tGeometry;
    plCsvReaderConfig.inputDir = inputDir;
    plCsvReaderConfig.inputHitsFileName = inputHitsFileName + ".csv";
    plCsvReaderConfig.inputDetailsFileName = inputDetailsFileName + ".csv";
    plCsvReaderConfig.inputTruthFileName = inputTruthFileName + ".csv";
    plCsvReaderConfig.outputClusterCollection = outputClusterCollection;
    plCsvReader 
      = std::make_shared<FW::Csv::CsvPlanarClusterReader>(
                                                          plCsvReaderConfig, logLevel);
  }

  // Output Csv directory
  std::string outputDir = vm["output-dir"].template as<std::string>();

  // Write back CsvPlanarClusterReader output as Csv files
  std::shared_ptr<FW::Csv::CsvPlanarClusterWriter> plCsvWriter = nullptr;
  if (vm["output-csv"].template as<bool>()) {
    FW::Csv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
    clusterWriterCsvConfig.collection = outputClusterCollection;
    clusterWriterCsvConfig.outputDir  = outputDir;
    plCsvWriter 
      = std::make_shared<FW::Csv::CsvPlanarClusterWriter>(
                                                          clusterWriterCsvConfig);
  }

  // Initiate the run
  if (particleCsvReader) sequencer.addReaders({particleCsvReader});
  if (plCsvReader) sequencer.addReaders({plCsvReader});
  if (plCsvWriter) sequencer.addWriters({plCsvWriter});
  sequencer.appendEventAlgorithms({});
  sequencer.run(nEvents);

  // Return 0 for success
  return 0;
}
