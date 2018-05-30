// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/DD4hep/DD4hepDetectorOptions.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "ExtrapolationExampleBase.hpp"

namespace po = boost::program_options;

/// The main executable
///
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
  // add the dd4hep detector options
  FW::Options::addDD4hepOptions<po::options_description>(desc);
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
  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  // read and create  ParticleGunConfig
  auto particleGunConfig
      = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  // read and create RandomNumbersConfig
  auto randomNumbersConfig
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  // read the detector config & dd4hep detector
  auto dd4HepDetectorConfig
      = FW::Options::readDD4hepConfig<po::variables_map>(vm);
  auto geometrySvc
      = std::make_shared<FW::DD4hep::GeometryService>(dd4HepDetectorConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
      = geometrySvc->trackingGeometry();

  // run the example - if you have a map run this, otherwise constant field
  return bField.first ? ACTFWExtrapolationExample::run(nEvents,
                                                       bField.first,
                                                       dd4tGeometry,
                                                       particleGunConfig,
                                                       randomNumbersConfig,
                                                       logLevel)
                      : ACTFWExtrapolationExample::run(nEvents,
                                                       bField.second,
                                                       dd4tGeometry,
                                                       particleGunConfig,
                                                       randomNumbersConfig,
                                                       logLevel);
}
