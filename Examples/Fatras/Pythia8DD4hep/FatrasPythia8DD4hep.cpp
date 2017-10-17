// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Fatras example with pythia8 generator and a DD4hep detector

#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>
#include <boost/program_options.hpp>
#include <cstdlib>
#include <iostream>
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/DD4hep/DD4hepDetectorOptions.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Generator.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Options.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // read the evgen options
  FW::Options::addEvgenOptions<po::options_description>(desc);
  // add the pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the dd4hep detector options
  FW::Options::addDD4hepOptions<po::options_description>(desc);
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents  = standardOptions.first;
  auto logLevel = standardOptions.second;
  // @todo add the output directory to the standard options
  std::string outputDir = "";
  // now read the bfield options
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  // now read the pythia8 configs
  auto pythia8Configs = FW::Options::readPythia8Config<po::variables_map>(vm);
  // the hard scatter generator
  auto hsPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      pythia8Configs.first,
      Acts::getDefaultLogger("HardScatterTPythia8Generator", logLevel));
  // the pileup generator
  auto puPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      pythia8Configs.second,
      Acts::getDefaultLogger("PileUpTPythia8Generator", logLevel));
  // Create the random number engine
  auto randomNumbersCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // now read the evgen config & set the missing parts
  auto readEvgenCfg                   = FW::Options::readEvgenConfig(vm);
  readEvgenCfg.hardscatterEventReader = hsPythiaGenerator;
  readEvgenCfg.pileupEventReader      = puPythiaGenerator;
  readEvgenCfg.randomNumbers          = randomNumbers;
  readEvgenCfg.barcodeSvc             = barcodeSvc;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FW::ReadEvgenAlgorithm>(
      readEvgenCfg, Acts::getDefaultLogger("ReadEvgenAlgorithm", logLevel));
  // read the detector config & dd4hep detector
  auto dd4HepDetectorConfig
      = FW::Options::readDD4hepConfig<po::variables_map>(vm);
  auto geometrySvc
      = std::make_shared<FW::DD4hep::GeometryService>(dd4HepDetectorConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
      = geometrySvc->trackingGeometry();

  // setup event loop
  FW::Sequencer sequencer({});
  if (sequencer.addReaders({readEvgen}) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (bField.first
      && setupSimulation(
             sequencer, dd4tGeometry, randomNumbers, bField.first, logLevel)
          != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  else if (setupSimulation(
               sequencer, dd4tGeometry, randomNumbers, bField.second, logLevel)
           != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (setupWriters(sequencer, barcodeSvc, outputDir)
      != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
