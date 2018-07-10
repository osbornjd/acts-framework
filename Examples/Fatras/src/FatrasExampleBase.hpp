// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Geometry/GeometryOptions.hpp"
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

/// The Fatras example
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param atgv the argument list
///
template <typename geometry_getter_t>
int
fatrasExample(int argc, char* argv[], geometry_getter_t trackingGeometry)
{

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // add the evgen options
  FW::Options::addEvgenReaderOptions<po::options_description>(desc);
  // add the pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add the fatras options
  FW::Options::addFatrasOptions<po::options_description>(desc);

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
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);
  // add it to the sequencer
  sequencer.addServices({randomNumberSvc});
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // add it to the sequencer
  sequencer.addServices({barcodeSvc});

  // (A) EVGEN
  // setup the evgen input to the simulation
  std::string evgenCollection = setupEvgenInput<po::variables_map>(
      vm, sequencer, barcodeSvc, randomNumberSvc);

  // get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // (B) SIMULATION
  // setup the simulation
  if (setupSimulation<po::variables_map>(vm,
                                         sequencer,
                                         tGeometry,
                                         barcodeSvc,
                                         randomNumberSvc,
                                         evgenCollection)
      != FW::ProcessCode::SUCCESS) {
    return -1;
  }

  // (Z) Writers
  // Root writer section
  if (setupRootWriters(vm, sequencer, barcodeSvc) != FW::ProcessCode::SUCCESS) {
    return -1;
  }

  // initiate the run
  sequencer.run(nEvents);
  return 1;
}
