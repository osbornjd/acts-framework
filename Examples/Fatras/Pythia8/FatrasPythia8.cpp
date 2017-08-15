/// @file
/// @brief Fatras example with a pythia8 generator and the generic detector

#include <cstdlib>

#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Generator.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"

#include "FatrasCommon.hpp"

int
main(int argc, char* argv[])
{
  size_t               nEvents   = 2;
  std::string          outputDir = ".";
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;

  // the barcode service
  auto barcode = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // random numbers
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  auto random   = std::make_shared<FW::RandomNumbersSvc>(brConfig);

  // TODO create a single pythia8 event generator w/o the need for this manual
  //      combination

  // pythia8 hard scatter generator
  // process: HardQCD for the moment
  FW::Pythia8::TPythia8Generator::Config hsPythiaConfig;
  hsPythiaConfig.pdgBeam0       = 2212;
  hsPythiaConfig.pdgBeam1       = 2212;
  hsPythiaConfig.cmsEnergy      = 14000.;
  hsPythiaConfig.processStrings = {{"HardQCD:all = on"}};
  auto hsPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      hsPythiaConfig,
      Acts::getDefaultLogger("HardScatterTPythia8Generator", logLevel));
  // pythia8 soft scatter generator
  // MinBias with SD, DD and ND
  FW::Pythia8::TPythia8Generator::Config puPythiaConfig;
  puPythiaConfig.pdgBeam0       = 2212;
  puPythiaConfig.pdgBeam1       = 2212;
  puPythiaConfig.cmsEnergy      = 14000.;
  puPythiaConfig.processStrings = {{"SoftQCD:all = on"}};
  auto puPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      puPythiaConfig,
      Acts::getDefaultLogger("PileUpTPythia8Generator", logLevel));
  // combined event generator
  FW::ReadEvgenAlgorithm::Config readEvgenCfg;
  readEvgenCfg.particlesCollection       = "Particles";
  readEvgenCfg.hardscatterParticleReader = hsPythiaGenerator;
  readEvgenCfg.pileupParticleReader      = puPythiaGenerator;
  readEvgenCfg.randomNumbers             = random;
  readEvgenCfg.pileupPoissonParameter    = 200;
  readEvgenCfg.vertexTParameters         = {{0., 0.015}};
  readEvgenCfg.vertexZParameters         = {{0., 5.5}};
  readEvgenCfg.barcodeSvc                = barcode;
  auto readEvgen = std::make_shared<FW::ReadEvgenAlgorithm>(
      readEvgenCfg, Acts::getDefaultLogger("Pythia8EventGenerator", logLevel));

  // generic detector as geometry
  std::shared_ptr<const Acts::TrackingGeometry> geom
      = FWGen::buildGenericDetector(logLevel, logLevel, logLevel, 3);

  // setup event loop
  FW::Sequencer sequencer({});
  if (sequencer.addReaders({readEvgen}) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (setupSimulation(sequencer, geom, random) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (setupWriters(sequencer, barcode, outputDir) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
