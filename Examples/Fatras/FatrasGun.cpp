/// @file
/// @brief Fatras example with a particle gun and the generic detector

#include <boost/program_options.hpp>
#include <cstdlib>
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
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
  // now read the standard options options
  auto standardOptions 
    = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents = standardOptions.first;
  auto logLevel = standardOptions.second;
  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);

  // the barcode service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // random numbers
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  auto random   = std::make_shared<FW::RandomNumbersSvc>(brConfig);

  using range = std::vector<double>;

  // particle gun as generator
  FW::ParticleGun::Config particleGunConfig;
  particleGunConfig.evgenCollection = "EvgenParticles";
  particleGunConfig.nParticles      = vm["nparticles"].as<size_t>();
  particleGunConfig.d0Range         = vm["d0range"].as<range>();
  particleGunConfig.z0Range         = vm["z0range"].as<range>();
  particleGunConfig.phiRange        = vm["phirange"].as<range>();
  particleGunConfig.etaRange        = vm["etarange"].as<range>();
  particleGunConfig.ptRange         = vm["ptrange"].as<range>();
  particleGunConfig.mass            = vm["mass"].as<double>() * _MeV;
  particleGunConfig.charge          = vm["charge"].as<double>() * _e;
  particleGunConfig.pID             = vm["pdg"].as<int>() * _MeV;
  particleGunConfig.randomNumbers   = random;
  particleGunConfig.randomCharge    = true;
  particleGunConfig.barcodes        = barcodes;
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
  if (bField.first && setupSimulation(sequencer, geom, random, bField.first, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  else if (setupSimulation(sequencer, geom, random, bField.second, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (setupWriters(sequencer, barcodes, outputDir, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
