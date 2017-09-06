#ifndef ACTFW_EXTRAPOLATION_EXAMPLEBASE_H
#define ACTFW_EXTRAPOLATION_EXAMPLEBASE_H

#include <memory>
#include <ACTS/MagneticField/ConstantBField.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

/// simple base for the extrapolation example
namespace ACTFWExtrapolationExample {

template <class MagneticField> 
int
run(size_t nEvents, 
    MagneticField magField,
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
    Acts::Logging::Level eLogLevel = Acts::Logging::INFO)
{
  using namespace Acts::units;

  if (!tGeometry) return -9;

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FW::initExtrapolator(tGeometry, magField, eLogLevel);
  
  // the barcode service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{},
      Acts::getDefaultLogger("BarcodeSvc", eLogLevel));

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbersSvc::Config          brConfig;
  std::shared_ptr<FW::RandomNumbersSvc> randomNumbers(
      new FW::RandomNumbersSvc(brConfig));

  // particle gun as generator
  FW::ParticleGun::Config particleGunConfig;
  particleGunConfig.evgenCollection     = "EvgenParticles";
  particleGunConfig.nParticles          = 100;
  particleGunConfig.d0Range             = {{0, 1 * _mm}};
  particleGunConfig.phiRange            = {{-M_PI, M_PI}};
  particleGunConfig.etaRange            = {{-4., 4.}};
  particleGunConfig.ptRange             = {{100 * _MeV, 100 * _GeV}};
  particleGunConfig.mass                = 105 * _MeV;
  particleGunConfig.charge              = -1 * _e;
  particleGunConfig.pID                 = 13;
  particleGunConfig.randomNumbers       = randomNumbers;
  particleGunConfig.barcodes            = barcodes;
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
     = std::make_shared<FW::Root::RootExCellWriter<Acts::TrackParameters > >
       (reccWriterConfig);

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
    = std::make_shared<FW::Root::RootExCellWriter<Acts::NeutralParameters > >
      (recnWriterConfig);

  // the Algorithm with its configurations
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.evgenCollection                   = particleGunConfig.evgenCollection;
  eTestConfig.simulatedParticlesCollection      = "SimulatedParticles";
  eTestConfig.simulatedChargedExCellCollection  = reccWriterConfig.collection;
  eTestConfig.simulatedNeutralExCellCollection  = recnWriterConfig.collection;
  eTestConfig.simulatedHitsCollection           = "SimulatedHits";
  eTestConfig.searchMode                        = 1;
  eTestConfig.extrapolationEngine               = extrapolationEngine;
  eTestConfig.randomNumbers                     = randomNumbers;
  eTestConfig.collectSensitive                  = true;
  eTestConfig.collectPassive                    = true;
  eTestConfig.collectBoundary                   = true;
  eTestConfig.collectMaterial                   = true;
  eTestConfig.sensitiveCurvilinear              = false;
  eTestConfig.pathLimit                         = -1.;

  auto extrapolationAlg
      = std::make_shared<FW::ExtrapolationAlgorithm>(eTestConfig, eLogLevel);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);  
  sequencer.addServices( {randomNumbers} );
  sequencer.addWriters( {rootEccWriter, rootEcnWriter} );
  sequencer.appendEventAlgorithms({particleGun, extrapolationAlg});
  sequencer.run(nEvents);

  return 0;
}
};  // namespace ACTFWExtrapolationExample

#endif  // ACTFW_EXTRAPOLATION_EXAMPLEBASE_H
