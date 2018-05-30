// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EXAMPLES_PARTICLEGUN_H
#define ACTFW_EXAMPLES_PARTICLEGUN_H

#include <boost/program_options.hpp>
#include <string>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
//#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/ParticleGun/ParticleGunAlgorithm.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

/// The main executable
///
/// Creates an instance of the particle gun algorithm
/// run particle generation and writes out

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // add specific options for this example
  desc.add_options()(
      "output-root-file",
      po::value<std::string>()->default_value(""),
      "If the string is not empty: write a '.root' output file (full run).")(
      "output-csv-file",
      po::value<std::string>()->default_value(""),
      "If the string is not empty: Write a '.csv' output files (per event)");
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

  // output root filename
  std::string rootFileName = vm["output-root-file"].get<std::string>();
  if (rootFileName != "" && rootFileName.find(".root"))

    // Write ROOT TTree
    FW::Root::RootParticleWriter::Config particleWriterConfig;
  particleWriterConfig.collection = readEvgenCfg.evgenCollection;
  particleWriterConfig.filePath   = "EvgenParticles.root";
  particleWriterConfig.barcodeSvc = barcodeSvc;
  auto particleWriter
      = std::make_shared<FW::Root::RootParticleWriter>(particleWriterConfig);

  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents  = standardOptions.first;
  auto logLevel = standardOptions.second;
  // Create the random number engine
  auto randomNumbersCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);
  // now read the particle gun configs
  auto particleGunCfg
      = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  auto particleGun = std::make_shared<FW::ParticleGunAlgorithm>(particleGunCfg);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({randomNumbers});
  sequencer.appendEventAlgorithms({particleGun});
  sequencer.addWriters({particleWriter});
  sequencer.run(nEvents);
}

#endif  // ACTFW_EXAMPLES_PARTICLEGUN_H
