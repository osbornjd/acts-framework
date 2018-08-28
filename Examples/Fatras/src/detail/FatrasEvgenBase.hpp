// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Pythia8/Generator.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "ACTFW/Utilities/Paths.hpp"

template <typename vmap_t>
void
setupEvgenInput(vmap_t&                               vm,
                FW::Sequencer&                        sequencer,
                std::shared_ptr<FW::BarcodeSvc>       barcodeSvc,
                std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc)
{
  // Read the standard options
  auto logLevel = FW::Options::readLogLevel<vmap_t>(vm);
  auto nEvents  = FW::Options::readNumberOfEvents<vmap_t>(vm);

  // Define which
  auto evgenInput = vm["evg-input-type"].template as<std::string>();

  using SignalReader = FW::IReaderT<std::vector<FW::Data::Vertex>>;

  // The signal & pile-up event reader
  std::shared_ptr<SignalReader> signalEventReader = nullptr;
  std::shared_ptr<SignalReader> pileupEventReader = nullptr;

  // Read the pythia8 configs
  auto pythia8Configs = FW::Options::readPythia8Config<vmap_t>(vm);

  // Check if the particle gun is to be used
  if (evgenInput == "gun") {

    // Read the particle gun configs
    auto particleGunCfg = FW::Options::readParticleGunConfig<vmap_t>(vm);

    // Are running in particle gun mode
    particleGunCfg.barcodeSvc      = barcodeSvc;
    particleGunCfg.randomNumberSvc = randomNumberSvc;
    particleGunCfg.nEvents         = nEvents;
    signalEventReader              = std::make_shared<FW::ParticleGun>(
        particleGunCfg, Acts::getDefaultLogger("ParticleGun", logLevel));
  } else {
    // Read Signal generator
    pythia8Configs.first.randomNumberSvc = randomNumberSvc;
    // The hard scatter generator
    signalEventReader = std::make_shared<FW::GPythia8::Generator>(
        pythia8Configs.first,
        Acts::getDefaultLogger("HardScatterPythia8Generator", logLevel));
  }

  if (vm["evg-pileup"].template as<int>()) {
    // The Pile-up generator - only if pilie-up is configured
    pythia8Configs.second.randomNumberSvc = randomNumberSvc;
    pileupEventReader = std::make_shared<FW::GPythia8::Generator>(
        pythia8Configs.second,
        Acts::getDefaultLogger("PileUpPythia8Generator", logLevel));
  }

  // Read the evgen config & set the missing parts
  auto readEvgenCfg                   = FW::Options::readEvgenConfig(vm);
  readEvgenCfg.hardscatterEventReader = signalEventReader;
  readEvgenCfg.pileupEventReader      = pileupEventReader;
  readEvgenCfg.randomNumberSvc        = randomNumberSvc;
  readEvgenCfg.barcodeSvc             = barcodeSvc;
  readEvgenCfg.nEvents                = nEvents;

  // Create the Evgen Reading Algorithm
  auto readEvgen = std::make_shared<FW::EvgenReader>(
      readEvgenCfg, Acts::getDefaultLogger("EvgenReader", logLevel));

  // Add readEvgen as a reader
  sequencer.addReaders({readEvgen});

  // Output directory
  std::string outputDir = vm["output-dir"].template as<std::string>();

  // Write particles as CSV files
  std::shared_ptr<FW::Csv::CsvParticleWriter> pWriterCsv = nullptr;
  if (vm["output-csv"].template as<bool>()) {
    FW::Csv::CsvParticleWriter::Config pWriterCsvConfig;
    pWriterCsvConfig.collection     = readEvgenCfg.evgenCollection;
    pWriterCsvConfig.outputDir      = outputDir;
    pWriterCsvConfig.outputFileName = readEvgenCfg.evgenCollection + ".csv";
    auto pWriterCsv
        = std::make_shared<FW::Csv::CsvParticleWriter>(pWriterCsvConfig);

    sequencer.addWriters({pWriterCsv});
  }

  // Write particles as ROOT file
  std::shared_ptr<FW::Root::RootParticleWriter> pWriterRoot = nullptr;
  if (vm["output-root"].template as<bool>()) {
    // Write particles as ROOT TTree
    FW::Root::RootParticleWriter::Config pWriterRootConfig;
    pWriterRootConfig.collection = readEvgenCfg.evgenCollection;
    pWriterRootConfig.filePath
        = FW::joinPaths(outputDir, readEvgenCfg.evgenCollection + ".root");
    pWriterRootConfig.treeName   = readEvgenCfg.evgenCollection;
    pWriterRootConfig.barcodeSvc = barcodeSvc;
    auto pWriterRoot
        = std::make_shared<FW::Root::RootParticleWriter>(pWriterRootConfig);

    sequencer.addWriters({pWriterRoot});
  }
}
