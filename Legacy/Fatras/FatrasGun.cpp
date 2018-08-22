// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Fatras example with a particle gun and the generic detector

#include <boost/program_options.hpp>

#include <cstdlib>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Digitization/DigitizationOptions.hpp"
#include "ACTFW/Fatras/FatrasAlgorithm.hpp"
#include "ACTFW/Fatras/FatrasOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Units.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  using namespace Acts::units;

  std::string outputDir = ".";

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // add the fatras options
  FW::Options::addFatrasOptions<po::options_description>(desc);
  // add the digitization options
  FW::Options::addDigitizationOptions<po::options_description>(desc);
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
  // read and create the magnetic field
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  // create the random number engine
  auto randomNumbersCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);
  // create the barcode service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // read and create ParticleGun config
  auto particleGunConfig
      = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  particleGunConfig.randomNumbers = randomNumbers;
  particleGunConfig.barcodes      = barcodes;
  auto particleGun
      = std::make_shared<FW::ParticleGun>(particleGunConfig, logLevel);

  // generic detector as geometry
  std::shared_ptr<const Acts::TrackingGeometry> geometry
      = FW::Generic::buildGenericDetector(logLevel, logLevel, logLevel, 3);

  // typedefs for the Propagation
  typedef Acts::ConstantBField                            BFieldC;
  typedef Acts::SharedBField<Acts::InterpolatedBFieldMap> BFieldI;
  typedef Acts::EigenStepper<BFieldC>                     EigenStepper_typeC;
  typedef Acts::EigenStepper<BFieldI>                     EigenStepper_typeI;

  typedef Acts::Propagator<EigenStepper_typeC> Propagator_typeC;
  typedef Acts::Propagator<EigenStepper_typeI> Propagator_typeI;

  // constant stepper
  BFieldC            fieldC(*bField.second);
  EigenStepper_typeC stepperC(std::move(fieldC));
  Propagator_typeC   propagatorC(std::move(stepperC));

  // interpolated stepper
  BFieldI            fieldI(bField.first);
  EigenStepper_typeI stepperI(std::move(fieldI));
  Propagator_typeI   propagatorI(std::move(stepperI));

  // read and create Fatras Configs
  auto fatrasConfig = FW::Options::readFatrasConfig(vm, propagatorI);
  fatrasConfig.trackingGeometry = geometry;
  fatrasConfig.randomNumbers    = randomNumbers;
  fatrasConfig.evgenCollection  = particleGunConfig.evgenCollection;
  // the fatras kernel
  auto fatrasKernel = std::make_shared<FW::FatrasAlgorithm<Propagator_typeI>>(
      fatrasConfig, logLevel);

  // it feeds into the fast digitization
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      pmStepperConfig, Acts::getDefaultLogger("PlanarModuleStepper", logLevel));

  auto digiConfig                   = FW::Options::readDigitizationConfig(vm);
  digiConfig.simulatedHitCollection = fatrasConfig.simulatedHitCollection;
  digiConfig.randomNumbers          = randomNumbers;
  digiConfig.planarModuleStepper    = pmStepper;
  auto digitization
      = std::make_shared<FW::DigitizationAlgorithm>(digiConfig, logLevel);

  // writer section
  // clusters as root
  FW::Root::RootPlanarClusterWriter::Config clusterWriterRootConfig;
  clusterWriterRootConfig.collection = digiConfig.clusterCollection;
  clusterWriterRootConfig.filePath   = "clusters.root";
  auto clusteWriterRoot = std::make_shared<FW::Root::RootPlanarClusterWriter>(
      clusterWriterRootConfig);

  // setup event loop
  FW::Sequencer sequencer({});
  if (sequencer.appendEventAlgorithms({particleGun, fatrasKernel, digitization})
      != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (sequencer.addWriters({clusteWriterRoot}) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  // run the event loop
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
