// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Common setup functions for fatras examples

#ifndef ACTFW_FATRASCOMMON_HPP
#define ACTFW_FATRASCOMMON_HPP

#include <memory>
#include <string>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"

//~ #include "ACTFW/SpacePointFinder/SpacePointFinder.hpp"

/// Setup extrapolation and digitization.
///
/// Expects a `EvgenParticles` object in the event store with the truth
/// particles.
template <class MagneticField>
FW::ProcessCode
setupSimulation(FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> geometry,
                std::shared_ptr<FW::RandomNumbersSvc>         random,
                std::shared_ptr<MagneticField>                bfield,
                Acts::Logging::Level loglevel = Acts::Logging::INFO)
{
  // extrapolation algorithm
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.evgenCollection              = "EvgenParticles";
  eTestConfig.simulatedParticlesCollection = "FatrasParticles";
  eTestConfig.simulatedHitsCollection      = "FatrasHits";
  eTestConfig.searchMode                   = 1;
  eTestConfig.extrapolationEngine
      = FW::initExtrapolator(geometry, bfield, loglevel);

  eTestConfig.skipNeutral          = true;
  eTestConfig.collectSensitive     = true;
  eTestConfig.collectPassive       = true;
  eTestConfig.collectBoundary      = true;
  eTestConfig.collectMaterial      = true;
  eTestConfig.sensitiveCurvilinear = false;
  eTestConfig.pathLimit            = -1.;
  auto extrapolationAlg
      = std::make_shared<FW::ExtrapolationAlgorithm>(eTestConfig, loglevel);

  // digitisation
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      pmStepperConfig, Acts::getDefaultLogger("PlanarModuleStepper", loglevel));

  FW::DigitizationAlgorithm::Config digConfig;
  digConfig.simulatedHitsCollection = eTestConfig.simulatedHitsCollection;
  digConfig.clustersCollection      = "FatrasClusters";
  digConfig.spacePointsCollection   = "FatrasSpacePoints";
  digConfig.randomNumbers           = random;
  digConfig.planarModuleStepper     = pmStepper;
  auto digitzationAlg = std::make_shared<FW::DigitizationAlgorithm>(
      digConfig, Acts::Logging::INFO);

  //~ FW::SpacePointFinder::Config spfConfig;
  //~ spfConfig.collectionIn = "FatrasClusters";
  //~ spfConfig.collectionOut = "SCTSpacePoints";
  //~ auto spacepointAlg = std::make_shared<FW::SpacePointFinder>(spfConfig,
  // Acts::Logging::DEBUG);

  // add algorithms to sequencer
  //~ if (sequencer.appendEventAlgorithms({extrapolationAlg, digitzationAlg,
  // spacepointAlg})
  if (sequencer.appendEventAlgorithms({extrapolationAlg, digitzationAlg})
      != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}

/// Setup writers to store simulation output
///
/// Expects `FatrasParticles`, `FatrasClusters`, `FatrasSpacePoints` objects
/// to be present in the event store.
FW::ProcessCode
setupWriters(FW::Sequencer&                  sequencer,
             std::shared_ptr<FW::BarcodeSvc> barcode,
             std::string                     outputDir,
             Acts::Logging::Level            loglevel = Acts::Logging::INFO);

#endif  // ACTFW_FATRASCOMMON_HPP
