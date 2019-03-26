// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <memory>

#include <boost/program_options.hpp>

#include <Acts/Detector/TrackingGeometry.hpp>
#include <Acts/MagneticField/ConstantBField.hpp>
#include <Acts/Utilities/Units.hpp>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Options/ParticleGunOptions.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Reconstruction/Empty.hpp"

using namespace boost::program_options;
using namespace FW;

int
main(int argc, char* argv[])
{
  // Setup command line arguments and options
  options_description desc("Gsoc2019 reconstruction tool");
  Options::addCommonOptions(desc);
  Options::addRandomNumbersOptions(desc);
  Options::addParticleGunOptions(desc);

  // Process command line arguments and options
  variables_map vm;
  store(parse_command_line(argc, argv, desc), vm);
  notify(vm);
  // Print help if reqested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  // Extract common options
  auto nEvents  = Options::readNumberOfEvents(vm);
  auto logLevel = Options::readLogLevel(vm);

  // Create a sequencer w/ the default config
  Sequencer seq({});

  // Setup basic services
  RandomNumbersSvc::Config rndCfg;
  rndCfg.seed  = 123;
  auto rng     = std::make_shared<RandomNumbersSvc>(rndCfg);
  auto barcode = std::make_shared<BarcodeSvc>(
      BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // Setup geometry. Always use the generic (TrackML) detector
  auto geo = Generic::buildGenericDetector(logLevel);

  // Always use a constant magnetic field
  Acts::ConstantBField bfield(0.0, 0.0, 2.0 * Acts::units::_T);

  // Event generation w/ particle gun
  EventGenerator::Config evgen = Options::readParticleGunOptions(vm);
  evgen.output                 = "particles";
  evgen.randomNumbers          = rng;
  evgen.barcodeSvc             = barcode;
  seq.addReaders({std::make_shared<EventGenerator>(evgen, logLevel)});

  // Add the empty reconstruction algorithm
  EmptyReconstructionAlgorithm::Config emptyReco;
  seq.appendEventAlgorithms(
      {std::make_shared<EmptyReconstructionAlgorithm>(emptyReco, logLevel)});

  return (seq.run(nEvents) == ProcessCode::SUCCESS) ? EXIT_SUCCESS
                                                    : EXIT_FAILURE;
}
