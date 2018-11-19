// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Pythia8/Generator.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Plugins/Root/RootTrackParameterWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "TrackSmearingAlgorithm.hpp"

namespace po = boost::program_options;

/// Main read evgen executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // Add the pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // Add the evgen options
  FW::Options::addEvgenReaderOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);
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
  // Read the common options
  auto nEvents  = FW::Options::readNumberOfEvents<po::variables_map>(vm);
  auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);
  // Now read the pythia8 configs
  auto pythia8Configs = FW::Options::readPythia8Config<po::variables_map>(vm);
  pythia8Configs.first.randomNumberSvc  = randomNumberSvc;
  pythia8Configs.second.randomNumberSvc = randomNumberSvc;
  // The hard scatter generator
  auto hsPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
      pythia8Configs.first,
      Acts::getDefaultLogger("HardScatterPythia8Generator", logLevel));
  // The pileup generator
  auto puPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
      pythia8Configs.second,
      Acts::getDefaultLogger("PileUpPythia8Generator", logLevel));
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // Now read the evgen config & set the missing parts
  auto readEvgenCfg                   = FW::Options::readEvgenConfig(vm);
  readEvgenCfg.hardscatterEventReader = hsPythiaGenerator;
  readEvgenCfg.pileupEventReader      = puPythiaGenerator;
  readEvgenCfg.randomNumberSvc        = randomNumberSvc;
  readEvgenCfg.barcodeSvc             = barcodeSvc;
  readEvgenCfg.nEvents                = nEvents;

  // Create the read Algorithm
  auto readEvgen = std::make_shared<FW::EvgenReader>(
      readEvgenCfg, Acts::getDefaultLogger("EvgenReader", logLevel));

  // Add the track smearing algorithm
  FWE::TrackSmearingAlgorithm::Config trksmrCfg;
  trksmrCfg.collection = readEvgenCfg.evgenCollection;
  trksmrCfg.randomNumberSvc = randomNumberSvc;
  //TODO: use make_shared instead of new
  std::shared_ptr<FW::IAlgorithm> trksmrAlgo(new FWE::TrackSmearingAlgorithm(trksmrCfg, logLevel));

  // Output directory
  std::string outputDir = vm["output-dir"].as<std::string>();

  // Write particles as CSV files
  std::shared_ptr<FW::Csv::CsvParticleWriter> pWriterCsv = nullptr;
  if (vm["output-csv"].as<bool>()) {
    FW::Csv::CsvParticleWriter::Config pWriterCsvConfig;
    pWriterCsvConfig.collection     = readEvgenCfg.evgenCollection;
    pWriterCsvConfig.outputDir      = outputDir;
    pWriterCsvConfig.outputFileName = readEvgenCfg.evgenCollection + ".csv";
    pWriterCsv = std::make_shared<FW::Csv::CsvParticleWriter>(pWriterCsvConfig);
  }

  // Write particles as ROOT file
  std::shared_ptr<FW::Root::RootParticleWriter> pWriterRoot = nullptr;
  if (vm["output-root"].as<bool>()) {
    // Write particles as ROOT TTree
    FW::Root::RootParticleWriter::Config pWriterRootConfig;
    pWriterRootConfig.collection = readEvgenCfg.evgenCollection;
    pWriterRootConfig.filePath   = readEvgenCfg.evgenCollection + ".root";
    pWriterRootConfig.barcodeSvc = barcodeSvc;
    pWriterRoot
        = std::make_shared<FW::Root::RootParticleWriter>(pWriterRootConfig);
  }

  std::shared_ptr<FW::Root::RootTrackParameterWriter> trkParamWriterRoot = nullptr;
  if (vm["output-trackparams"].as<bool>()) {
    FW::Root::RootTrackParameterWriter::Config trkParamWriterRootConfig;
    trkParamWriterRootConfig.collection = trksmrCfg.collectionOut;
    trkParamWriterRootConfig.filePath   = trksmrCfg.collectionOut + ".root";
    trkParamWriterRoot = std::make_shared<FW::Root::RootTrackParameterWriter>(trkParamWriterRootConfig);
  }
  


  // Create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // Now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({randomNumberSvc});
  sequencer.addReaders({readEvgen});
  sequencer.appendEventAlgorithms({trksmrAlgo});
  if (pWriterRoot) sequencer.addWriters({pWriterRoot});
  if (pWriterCsv) sequencer.addWriters({pWriterCsv});
  if (trkParamWriterRoot) sequencer.addWriters({trkParamWriterRoot});
  sequencer.run(nEvents);

  return 0;
}
