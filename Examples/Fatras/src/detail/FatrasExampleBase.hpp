// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#include <boost/program_options.hpp>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/GeometryOptions.hpp"
#include "ACTFW/Common/MaterialOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/Digitization/DigitizationOptions.hpp"
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Options/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "Acts/Material/IMaterialDecorator.hpp"
#include "FatrasDigitizationBase.hpp"
#include "FatrasEvgenBase.hpp"
#include "FatrasSimulationBase.hpp"

/// @brief The Fatras example
///
/// This instantiates the geometry and runs fast track simultion
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
fatrasExample(int               argc,
              char*             argv[],
              options_setup_t&  optionsSetup,
              geometry_setup_t& geometrySetup)
{

  // Create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // Now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the Common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // Add the material options
  FW::Options::addMaterialOptions<po::options_description>(desc);
  // Add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // Add the Pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // Add the evgen options
  FW::Options::addEvgenReaderOptions<po::options_description>(desc);
  // Add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // Add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // Add the fatras options
  FW::Options::addFatrasOptions<po::options_description>(desc);
  // Add the digization options
  FW::Options::addDigitizationOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);
  // Add program specific options: input / output
  desc.add_options()("evg-input-type",
                     value<std::string>()->default_value("pythia8"),
                     "Type of evgen input 'gun', 'pythia8'");
  // Add specific options for this geometry
  optionsSetup(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  auto logLevel = FW::Options::readLogLevel(vm);

  // Create the random number engine
  auto randomNumberSvcCfg = FW::Options::readRandomNumbersConfig(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);

  // Add it to the sequencer
  sequencer.addService(randomNumberSvc);
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // Add it to the sequencer
  sequencer.addService(barcodeSvc);

  // Material loading from external source
  auto mDecorator = FW::Options::readMaterialDecorator<po::variables_map>(vm);

  // Create the geometry and the context decorators
  auto geometry          = geometrySetup(vm, mDecorator);
  auto tGeometry         = geometry.first;
  auto contextDecorators = geometry.second;

  // Add it to the sequencer
  for (auto cdr : contextDecorators) {
    sequencer.addContextDecorator(cdr);
  }

  // (A) EVGEN
  // Setup the evgen input to the simulation
  setupEvgenInput(vm, sequencer, barcodeSvc, randomNumberSvc);

  // (B) SIMULATION
  // Setup the simulation
  setupSimulation(vm, sequencer, tGeometry, barcodeSvc, randomNumberSvc);

  // (C) DIGITIZATION
  // Setup the digitization
  setupDigitization(vm, sequencer, barcodeSvc, randomNumberSvc);

  // (D) TRUTH TRACKING

  // (E) PATTERN RECOGNITION

  return sequencer.run();
}
