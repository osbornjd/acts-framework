// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EXTRAPOLATION_EXAMPLEBASE_H
#define ACTFW_EXTRAPOLATION_EXAMPLEBASE_H

#include <ACTS/MagneticField/ConstantBField.hpp>
#include <ACTS/Utilities/Units.hpp>
#include <memory>
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "FatrasCommon.hpp"

/// simple base for the extrapolation example
namespace ACTFWExtrapolationExample {

template <class MagneticField>
int
run(size_t                                        nEvents,
    MagneticField                                 magField,
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
    FW::ParticleGun::Config&                      particleGunConfig,
    FW::RandomNumbersSvc::Config&                 randomNumbersConfig,
    Acts::Logging::Level eLogLevel = Acts::Logging::INFO)
{
  using namespace Acts::units;

  if (!tGeometry) return -9;

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FW::initExtrapolator(tGeometry, magField, eLogLevel);

  // THE BARCODE service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{},
      Acts::getDefaultLogger("BarcodeSvc", eLogLevel));

  // RANDOM NUMBERS - Create the random number engine
  std::shared_ptr<FW::RandomNumbersSvc> randomNumbers(
      new FW::RandomNumbersSvc(randomNumbersConfig));

  // set the random
  particleGunConfig.randomNumbers = randomNumbers;
  particleGunConfig.barcodes      = barcodes;

  auto particleGun
      = std::make_shared<FW::ParticleGun>(particleGunConfig, eLogLevel);

  // Write ROOT TTree
  // ecc for charged particles
  FW::Root::RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.filePath       = "excells_charged.root";
  reccWriterConfig.treeName       = "extrapolation_charged";
  reccWriterConfig.collection     = "excells_charged";
  reccWriterConfig.writeBoundary  = false;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  auto rootEccWriter
      = std::make_shared<FW::Root::RootExCellWriter<Acts::TrackParameters>>(
          reccWriterConfig);

  // ecc for neutral particles
  FW::Root::RootExCellWriter<Acts::NeutralParameters>::Config recnWriterConfig;
  recnWriterConfig.filePath       = "excells_neutral.root";
  recnWriterConfig.treeName       = "extrapolation_neutral";
  recnWriterConfig.collection     = "excells_neutral";
  recnWriterConfig.writeBoundary  = false;
  recnWriterConfig.writeMaterial  = true;
  recnWriterConfig.writeSensitive = true;
  recnWriterConfig.writePassive   = true;
  auto rootEcnWriter
      = std::make_shared<FW::Root::RootExCellWriter<Acts::NeutralParameters>>(
          recnWriterConfig);

  // the Algorithm with its configurations
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.evgenCollection              = particleGunConfig.evgenCollection;
  eTestConfig.simulatedParticlesCollection = "SimulatedParticles";
  eTestConfig.simulatedChargedExCellCollection = reccWriterConfig.collection;
  eTestConfig.simulatedNeutralExCellCollection = recnWriterConfig.collection;
  eTestConfig.simulatedHitsCollection          = "SimulatedHits";
  eTestConfig.searchMode                       = 1;
  eTestConfig.extrapolationEngine              = extrapolationEngine;
  eTestConfig.collectSensitive                 = true;
  eTestConfig.collectPassive                   = true;
  eTestConfig.collectBoundary                  = true;
  eTestConfig.collectMaterial                  = true;
  eTestConfig.sensitiveCurvilinear             = false;
  eTestConfig.pathLimit                        = -1.;

  auto extrapolationAlg
      = std::make_shared<FW::ExtrapolationAlgorithm>(eTestConfig, eLogLevel);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  seqConfig.jobStoreLogLevel   = Acts::Logging::INFO;
  seqConfig.eventStoreLogLevel = Acts::Logging::INFO;

  // now create the sequencer
  FW::Sequencer sequencer(
      seqConfig, Acts::getDefaultLogger("Squencer", Acts::Logging::INFO));
  sequencer.addServices({randomNumbers});
  setupWriters(sequencer, barcodes, "", Acts::Logging::INFO);
  //~ sequencer.addWriters({rootEccWriter, rootEcnWriter});
  sequencer.prependEventAlgorithms({particleGun});
  setupSimulation(
      sequencer, tGeometry, randomNumbers, magField, Acts::Logging::INFO);

  //~ sequencer.appendEventAlgorithms({particleGun, extrapolationAlg});
  sequencer.run(nEvents);

  return 0;
}
};  // namespace ACTFWExtrapolationExample

#endif  // ACTFW_EXTRAPOLATION_EXAMPLEBASE_H
