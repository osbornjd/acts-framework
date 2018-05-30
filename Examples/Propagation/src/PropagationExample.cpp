// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Root/RootTrackParametersWriter.hpp"
#include "ACTFW/Propagation/PropagationAlgorithm.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/Extrapolation/RungeKuttaEngine.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBFieldMap.hpp"
#include "Acts/Propagator/AtlasStepper.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Utilities/Units.hpp"

namespace po = boost::program_options;

// the main executable
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 100, 2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add an output file
  desc.add_options()("prop-covtransport",
                     po::value<bool>()->default_value(true),
                     "Covariance matrix transport on/off.")(
      "prop-caching",
      po::value<bool>()->default_value(true),
      "Caching the mangetic field.")(
      "prop-testmode",
      po::value<size_t>()->default_value(0),
      "Testmodes are 0 : path length, 1 : kalman.");
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
  // read and create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  // configuration check
  if (!bField.first) {
    std::cout << "Configuration error: no magnetic field ! " << std::endl;
    std::cout << "A magnetic field map at input is required." << std::endl;
    return -9;
  }

  // read and create RandomNumbersConfig
  auto randomNumbersConfig
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  // create the barcode service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // create the reandom number service
  auto randomNumbers
      = std::make_shared<FW::RandomNumbersSvc>(randomNumbersConfig);
  // read and create  ParticleGunConfig
  auto particleGunConfig
      = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  // set the random numbers and the barcode service
  particleGunConfig.randomNumbers = randomNumbers;
  particleGunConfig.barcodes      = barcodes;
  auto particleGun
      = std::make_shared<FW::ParticleGun>(particleGunConfig, logLevel);

  // create the shared field
  using BField = Acts::SharedBField<Acts::InterpolatedBFieldMap>;
  BField fieldA(bField.first);
  BField fieldB(bField.first);

  using StepperA = Acts::EigenStepper<BField>;
  using StepperB = Acts::AtlasStepper<BField>;
  using PropA    = Acts::Propagator<StepperA>;
  using PropB    = Acts::Propagator<StepperB>;
  using PropE    = Acts::RungeKuttaEngine<Acts::InterpolatedBFieldMap>;

  auto stepperA = StepperA(fieldA);
  auto stepperB = StepperB(fieldB);

  using PropagationTest = FW::PropagationAlgorithm<PropA, PropB, PropE>;

  // the Algorithm with its configurations
  typename PropagationTest::Config propConfig;
  propConfig.evgenCollection           = particleGunConfig.evgenCollection;
  propConfig.trackParametersCollection = "PropagatedParameters";

  // random numbers and covariance setting
  propConfig.randomNumbers = randomNumbers;

  // the configuration of the covariance number transport
  propConfig.covarianceTransport = vm["prop-covtransport"].as<bool>();
  if (propConfig.covarianceTransport) {
    Acts::ActsVectorD<5> covariances;
    covariances << 0.015, 0.055, 0.001, 0.001, 0.01;
    propConfig.covariances = covariances;
    /// the correlation terms
    propConfig.correlations(0, 2) = propConfig.correlations(2, 0) = 0.75;
    propConfig.correlations(0, 4) = propConfig.correlations(4, 0) = 0.4;
    propConfig.correlations(1, 3) = propConfig.correlations(3, 1) = 0.6;
    propConfig.correlations(1, 4) = propConfig.correlations(4, 1) = 0.2;
    propConfig.correlations(3, 4) = propConfig.correlations(4, 3) = 0.15;
  }

  // create the propagators with new stepper interface
  propConfig.propagatorA = std::make_shared<PropA>(stepperA);
  propConfig.propagatorB = std::make_shared<PropB>(stepperB);

  // create the ATLAS propagator with old interface
  typename PropE::Config pConfigE{};
  pConfigE.fieldService  = bField.first;
  propConfig.propagatorE = std::make_shared<PropE>(pConfigE);

  // the magnetic field type
  size_t tMode = vm["prop-testmode"].as<size_t>();
  if (tMode == 0)
    propConfig.testMode = (PropagationTest::TestMode)tMode;
  else
    propConfig.testMode = PropagationTest::kalman;
  // path limit for propagation without surface
  // (a) path limit test
  propConfig.pathLimit = 1. * Acts::units::_m;
  // (b) kalman filter test
  propConfig.cacheCall     = vm["prop-caching"].as<bool>();
  propConfig.cylinderRadii = {25. * Acts::units::_mm,
                              55. * Acts::units::_mm,
                              85. * Acts::units::_mm,
                              125. * Acts::units::_mm,
                              185. * Acts::units::_mm,
                              250. * Acts::units::_mm,
                              350. * Acts::units::_mm,
                              650. * Acts::units::_mm,
                              800. * Acts::units::_mm,
                              1000. * Acts::units::_mm};

  // propagtion algorithm
  auto propagationAlg = std::make_shared<PropagationTest>(propConfig, logLevel);

  typedef std::unique_ptr<const Acts::TrackParameters> TrackParametersPtr;
  typedef std::vector<TrackParametersPtr>              tp_vector;

  FW::Root::RootTrackParametersWriter<tp_vector>::Config tpWriterConfig;
  tpWriterConfig.filePath   = "track_parameters.root";
  tpWriterConfig.treeName   = "track_parameters";
  tpWriterConfig.collection = propConfig.trackParametersCollection;
  auto rootTpWriter
      = std::make_shared<FW::Root::RootTrackParametersWriter<tp_vector>>(
          tpWriterConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;

  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({randomNumbers});
  sequencer.addWriters({rootTpWriter});
  sequencer.appendEventAlgorithms({particleGun, propagationAlg});
  sequencer.run(nEvents);

  return 0;
}
