/// @file
/// @brief Fatras example with a particle gun and a DD4hep detector

#include <cstdlib>
#include <iostream>

#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>

#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"

#include "FatrasCommon.hpp"

int
main(int argc, char* argv[])
{
  using namespace Acts::units;

  size_t               nEvents = 1;
  std::string          detectorPath;
  std::string          outputDir = ".";
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;

  if (1 < argc) {
    detectorPath = argv[1];
  } else {
    detectorPath
        = "file:Detectors/DD4hepDetector/compact/FCChhTrackerTkLayout.xml";
  }
  std::cout << "Creating detector from '" << detectorPath << "'" << std::endl;

  // the barcodes service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // random numbers
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  auto random   = std::make_shared<FW::RandomNumbersSvc>(brConfig);

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
  particleGunConfig.randomNumbers       = random;
  particleGunConfig.barcodes            = barcodes;
  auto particleGun
      = std::make_shared<FW::ParticleGun>(particleGunConfig, logLevel);

  // geometry from dd4hep
  FWDD4hep::GeometryService::Config gsConfig("GeometryService", logLevel);
  gsConfig.xmlFileName              = detectorPath;
  gsConfig.bTypePhi                 = Acts::equidistant;
  gsConfig.bTypeR                   = Acts::equidistant;
  gsConfig.bTypeZ                   = Acts::equidistant;
  gsConfig.envelopeR                = 0.;
  gsConfig.envelopeZ                = 0.;
  gsConfig.buildDigitizationModules = false;
  auto geometrySvc = std::make_shared<FWDD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> geom
      = geometrySvc->trackingGeometry();

  // setup event loop
  FW::Sequencer sequencer({});
  if (sequencer.prependEventAlgorithms({particleGun})
      != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (setupSimulation(sequencer, geom, random) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (setupWriters(sequencer, barcodes, outputDir) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
