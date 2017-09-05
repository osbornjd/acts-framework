/// @file
/// @brief Fatras example with a particle gun and the generic detector

#include <boost/program_options.hpp>
#include <cstdlib>
#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/BField/BFieldFromFile.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  using namespace Acts::units;

  std::string          outputDir = ".";

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
      "events,n",
      po::value<size_t>()->default_value(1000),
      "The number of events to be processed")(
      "loglevel,l",
      po::value<size_t>()->default_value(2),
      "The output log level.");
  FW::BField::bFieldOptions<po::options_description>(desc);    
  
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  // output messages
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // create BField service
  auto bField = FW::BField::bFieldFromFile<po::variables_map>(vm);
  
  size_t nEvents = 1000;
  if (vm.count("events")){
    nEvents = vm["events"].as<size_t>();
    std::cout << "- running " << nEvents << " events "<< std::endl;
  } else {
    std::cout << "- using standard number of events " << nEvents << std::endl;
  }
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;  
  if (vm.count("loglevel")) {
    logLevel =   Acts::Logging::Level(vm["loglevel"].as<size_t>());    
    std::cout << "- the output log level is set to " << logLevel << std::endl;
  } else {
    std::cout << "- default log level is " << logLevel << std::endl;
  }
  double bscalor = 1.;
  if (vm.count("bscalor")) {
    bscalor = vm["bscalor"].template as<double>();
    std::cout << "- BField scalor to Tesla set to: " << bscalor << std::endl;
  }
  std::shared_ptr<Acts::ConstantBField> cField 
    = std::make_shared<Acts::ConstantBField>(0.,0.,bscalor* Acts::units::_T);

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
  if (bField && setupSimulation(sequencer, geom, random, bField, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  else if (setupSimulation(sequencer, geom, random, cField, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (setupWriters(sequencer, barcodes, outputDir, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
