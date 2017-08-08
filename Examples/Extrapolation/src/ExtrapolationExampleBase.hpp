#ifndef ACTFW_EXTRAPOLATION_EXAMPLEBASE_H
#define ACTFW_EXTRAPOLATION_EXAMPLEBASE_H

#include <memory>

#include <ACTS/MagneticField/ConstantBField.hpp>
#include <ACTS/Utilities/Units.hpp>

#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

/// simple base for the extrapolation example
namespace ACTFWExtrapolationExample {

int
run(size_t nEvents, std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{
  using namespace Acts::units;

  if (!tGeometry) return -9;

  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
      new Acts::ConstantBField{{0., 0., 2. * Acts::units::_T}});

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
  particleGunConfig.particlesCollection = "Particles";
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
  FWRoot::RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.fileName       = "$PWD/ExtrapolationChargedTest.root";
  reccWriterConfig.treeName       = "ExtrapolationChargedTest";
  reccWriterConfig.writeBoundary  = false;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters>>>
      rootEccWriter(new FWRoot::RootExCellWriter<Acts::TrackParameters>(
          reccWriterConfig));

  // ecc for neutral particles
  FWRoot::RootExCellWriter<Acts::NeutralParameters>::Config recnWriterConfig;
  recnWriterConfig.fileName       = "$PWD/ExtrapolationNeutralTest.root";
  recnWriterConfig.treeName       = "ExtrapolationNeutralTest";
  recnWriterConfig.writeBoundary  = false;
  recnWriterConfig.writeMaterial  = true;
  recnWriterConfig.writeSensitive = true;
  recnWriterConfig.writePassive   = true;
  std::shared_ptr<
      FW::IWriterT<Acts::ExtrapolationCell<Acts::NeutralParameters>>>
      rootEcnWriter(new FWRoot::RootExCellWriter<Acts::NeutralParameters>(
          recnWriterConfig));

  // the Algorithm with its configurations
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.particlesCollection          = "Particles";
  eTestConfig.simulatedParticlesCollection = "simulatedParticles";
  eTestConfig.simulatedHitsCollection      = "simulatedHits";
  eTestConfig.searchMode                   = 1;
  eTestConfig.extrapolationEngine          = extrapolationEngine;
  eTestConfig.ecChargedWriter              = rootEccWriter;
  eTestConfig.ecNeutralWriter              = rootEcnWriter;
  eTestConfig.randomNumbers                = randomNumbers;
  eTestConfig.collectSensitive             = true;
  eTestConfig.collectPassive               = true;
  eTestConfig.collectBoundary              = true;
  eTestConfig.collectMaterial              = true;
  eTestConfig.sensitiveCurvilinear         = false;
  eTestConfig.pathLimit                    = -1.;

  auto extrapolationAlg
      = std::make_shared<FW::ExtrapolationAlgorithm>(eTestConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({rootEccWriter, rootEcnWriter, randomNumbers});
  sequencer.appendEventAlgorithms({particleGun, extrapolationAlg});
  sequencer.run(nEvents);

  return 0;
}
};  // namespace ACTFWExtrapolationExample

#endif  // ACTFW_EXTRAPOLATION_EXAMPLEBASE_H
