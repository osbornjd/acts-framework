/// @file
/// @brief Fatras example with a particle gun and the generic detector

#include <cstdlib>

#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>

#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"

#include "FatrasCommon.hpp"

int
main(int argc, char* argv[])
{
  using namespace Acts::units;

  size_t               nEvents   = 1000;
  std::string          outputDir = ".";
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;

  // the barcode service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // random numbers
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  auto random   = std::make_shared<FW::RandomNumbersSvc>(brConfig);

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
  particleGunConfig.randomNumbers       = random;
  particleGunConfig.randomCharge        = true;
  particleGunConfig.barcodes            = barcodes;
  auto particleGun
      = std::make_shared<FW::ParticleGun>(particleGunConfig, logLevel);

  // generic detector as geometry
  std::shared_ptr<const Acts::TrackingGeometry> geom
      = FWGen::buildGenericDetector(logLevel, logLevel, logLevel, 3);

  // setup event loop
  FW::Sequencer sequencer({});
  if (sequencer.prependEventAlgorithms({particleGun})
      != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (setupSimulation(sequencer, geom, random, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (setupWriters(sequencer, barcodes, outputDir, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
