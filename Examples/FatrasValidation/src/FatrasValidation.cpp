// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include "ACTFW/FatrasValidation/G4FatrasValidation.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/DD4hep/DD4hepDetectorOptions.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "ACTFW/Plugins/Root/MaterialEffectsWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "TROOT.h"

namespace po = boost::program_options;

/// This example initializes and processes the Geant4 simulation to validate
/// Fatras simulation against Geant4. It writes out the particle properties of
/// the first and the last step in order to compare energy loss and displacement
/// due to material effects.

int
main(int argc, char* argv[])
{
  // enable root thread safety in order to use root writers in multi threaded
  // mode
  ROOT::EnableThreadSafety();

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 100, 2);
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

  // Translate DD4hep geometry to geant4
  FW::DD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4",
                                               Acts::Logging::INFO);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc = std::make_shared<FW::DD4hepG4::DD4hepToG4Svc>(dgConfig);

  // set up the particle properties writer
  FW::Root::MaterialEffectsWriter::Config writerConfig;
  writerConfig.collection = "particleProperties";
  writerConfig.treeName   = "G4particleProperties";
  writerConfig.filePath   = "G4particleProperties.root";
  auto writer = std::make_shared<FW::Root::MaterialEffectsWriter>(writerConfig);

  // @todo make program options for that
  // The configuration of the PrimaryGeneratorAction
  FW::G4::PrimaryGeneratorAction::Config pgaConfig;
  pgaConfig.nParticles   = 1;
  pgaConfig.particleName = "mu-";
  pgaConfig.energy       = 105.;
  pgaConfig.ptRange      = {10. * Acts::units::_GeV, 10. * Acts::units::_GeV};

  // @todo make program options for that
  // Simulation algorithm
  FW::G4::G4FatrasValidation::Config algConfig;
  algConfig.geant4Service                = dd4hepToG4Svc;
  algConfig.particlePropertiesCollection = "particleProperties";
  algConfig.testsPerEvent                = nEvents;
  algConfig.radialStepLimit              = 1000. * Acts::units::_mm;
  algConfig.pgaConfig                    = pgaConfig;
  auto algorithm = std::make_shared<FW::G4::G4FatrasValidation>(algConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({geometrySvc, dd4hepToG4Svc});
  sequencer.addWriters({writer});
  sequencer.appendEventAlgorithms({algorithm});
  sequencer.run(nEvents);

  return 0;
}
