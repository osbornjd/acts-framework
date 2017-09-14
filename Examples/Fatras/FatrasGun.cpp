/// @file
/// @brief Fatras example with a particle gun and the generic detector

#include <boost/program_options.hpp>
#include <cstdlib>
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  using namespace Acts::units;

  std::string          outputDir = ".";

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,1,2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);   
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);                     
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
  auto nEvents = standardOptions.first;
  auto logLevel = standardOptions.second;
  // read and create the magnetic field
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  // read and create  ParticleGunConfig
  auto particleGunConfig 
    = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  // create the barcode service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));
  // create the random number engine
  auto randomNumbersCfg =
    FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);
  particleGunConfig.randomNumbers   = randomNumbers;
  particleGunConfig.barcodes        = barcodes;
  auto particleGun
      = std::make_shared<FW::ParticleGun>(particleGunConfig, logLevel);

  // generic detector as geometry
  std::shared_ptr<const Acts::TrackingGeometry> geometry
      = FWGen::buildGenericDetector(logLevel, logLevel, logLevel, 3);

  // setup event loop
  FW::Sequencer sequencer({});
  if (sequencer.prependEventAlgorithms({particleGun})
      != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (bField.first && setupSimulation(sequencer, geometry, randomNumbers, bField.first, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  else if (setupSimulation(sequencer, geometry, randomNumbers, bField.second, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (setupWriters(sequencer, barcodes, outputDir, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
