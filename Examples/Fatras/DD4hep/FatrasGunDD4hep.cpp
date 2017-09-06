/// @file
/// @brief Fatras example with a particle gun and a DD4hep detector

#include <cstdlib>
#include <iostream>
#include <boost/program_options.hpp>
#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  using namespace Acts::units;

  std::string          detectorPath;
  std::string          outputDir = ".";
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()(
      "input",
      po::value<std::string>()->default_value(
        "file:Detectors/DD4hepDetector/compact/FCChhTrackerTkLayout.xml"),
      "The location of the input DD4hep file, use 'file:foo.xml'");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,1,2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);          
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // read the standard options
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

  // the barcodes service
  auto barcodes = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // random numbers
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  auto random   = std::make_shared<FW::RandomNumbersSvc>(brConfig);

  // particle gun as generator
  FW::ParticleGun::Config particleGunConfig;
  particleGunConfig.evgenCollection = "Particles";
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
  FW::DD4hep::GeometryService::Config gsConfig("GeometryService", logLevel);
  gsConfig.xmlFileName              = vm["input"].as<std::string>();
  gsConfig.bTypePhi                 = Acts::equidistant;
  gsConfig.bTypeR                   = Acts::equidistant;
  gsConfig.bTypeZ                   = Acts::equidistant;
  gsConfig.envelopeR                = 0.;
  gsConfig.envelopeZ                = 0.;
  //gsConfig.buildDigitizationModules = false;
  auto geometrySvc = std::make_shared<FW::DD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> geom
      = geometrySvc->trackingGeometry();

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
