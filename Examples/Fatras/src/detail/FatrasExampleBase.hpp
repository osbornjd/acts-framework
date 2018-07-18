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
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Plugins/Root/RootFatrasHitWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "FatrasEvgenBase.hpp"
#include "FatrasSimulationBase.hpp"
#include "FatrasWriterBase.hpp"

namespace po = boost::program_options;

/// @brief The Fatras example
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param atgv the argument list
///
template <typename geometry_options_t, typename geometry_getter_t>
int
fatrasExample(int                argc,
              char*              argv[],
              geometry_options_t geometryOptions,
              geometry_getter_t  trackingGeometry)
{
  // Create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // Now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the Common options
  FW::Options::addCommonOptions<po::options_description>(desc, 1, 2);
  // Add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // Add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // Add the evgen options
  FW::Options::addEvgenReaderOptions<po::options_description>(desc);
  // Add the Pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // Add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // Add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // Add the fatras options
  FW::Options::addFatrasOptions<po::options_description>(desc);
  // Add program specific options
  desc.add_options()("fatras-evgen-input",
                     po::value<std::string>()->default_value("pythia"),
                     "Type of evgen input 'gun', 'pythia' (default).");

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

  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);

  // Add it to the sequencer
  sequencer.addServices({randomNumberSvc});
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // Add it to the sequencer
  sequencer.addServices({barcodeSvc});

  // (A) EVGEN
  // Setup the evgen input to the simulation
  std::string evgenCollection = setupEvgenInput<po::variables_map>(
      vm, sequencer, barcodeSvc, randomNumberSvc);

  // get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // (B) SIMULATION
  // Setup the simulation
  if (setupSimulation<po::variables_map>(
          vm, sequencer, tGeometry, barcodeSvc, randomNumberSvc)
      != FW::ProcessCode::SUCCESS) {
    return -1;
  }

  // (C) Digitisation

  // (D) Truth tracking

  // (E) Pattern recognition

  // (Z) Writers
  // Root writer section
  if (setupRootWriters(vm, sequencer, barcodeSvc) != FW::ProcessCode::SUCCESS) {
    return -1;
  }

  // Initiate the run
  sequencer.run(nEvents);
  // Return 0 for success
  return 0;
}
