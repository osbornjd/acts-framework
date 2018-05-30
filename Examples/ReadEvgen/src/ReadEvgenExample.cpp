// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Pythia8/Generator.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"

namespace po = boost::program_options;

// the main read evgen executable
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the evgen options
  FW::Options::addEvgenOptions<po::options_description>(desc);
  // add the pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
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
  // Create the random number engine
  auto randomNumbersCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);
  // now read the pythia8 configs
  auto pythia8Configs = FW::Options::readPythia8Config<po::variables_map>(vm);
  pythia8Configs.first.randomNumbers  = randomNumbers;
  pythia8Configs.second.randomNumbers = randomNumbers;
  // the hard scatter generator
  auto hsPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
      pythia8Configs.first,
      Acts::getDefaultLogger("HardScatterPythia8Generator", logLevel));
  // the pileup generator
  auto puPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
      pythia8Configs.second,
      Acts::getDefaultLogger("PileUpPythia8Generator", logLevel));
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
  readEvgenCfg.nEvents                = nEvents;

  // create the read Algorithm
  auto readEvgen = std::make_shared<FW::ReadEvgenAlgorithm>(
      readEvgenCfg, Acts::getDefaultLogger("ReadEvgenAlgorithm", logLevel));

  // Write ROOT TTree
  FW::Root::RootParticleWriter::Config particleWriterConfig;
  particleWriterConfig.collection = readEvgenCfg.evgenCollection;
  particleWriterConfig.filePath   = "EvgenParticles.root";
  particleWriterConfig.barcodeSvc = barcodeSvc;
  auto particleWriter
      = std::make_shared<FW::Root::RootParticleWriter>(particleWriterConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({randomNumbers});
  sequencer.addReaders({readEvgen});
  sequencer.appendEventAlgorithms({});
  sequencer.addWriters({particleWriter});
  sequencer.run(nEvents);
}
