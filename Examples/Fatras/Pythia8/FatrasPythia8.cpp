/// @file
/// @brief Fatras example with a pythia8 generator and the generic detector

#include <cstdlib>
#include <boost/program_options.hpp>
#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Generator.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,1,2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);  
  // read the evgen options
  FW::Options::addEvgenOptions<po::options_description>(desc);         
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

  // output directory
  std::string          outputDir = ".";
  
  // the barcode service
  auto barcode = std::make_shared<FW::BarcodeSvc>(
      FW::BarcodeSvc::Config{}, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // random numbers
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  auto random   = std::make_shared<FW::RandomNumbersSvc>(brConfig);

  // TODO create a single pythia8 event generator w/o the need for this manual
  //      combination
  FW::Pythia8::TPythia8Generator::Config hsPythiaConfig;
  hsPythiaConfig.pdgBeam0       = vm["pdgBeam0"].as<int>();
  hsPythiaConfig.pdgBeam1       = vm["pdgBeam1"].as<int>();
  hsPythiaConfig.cmsEnergy      = vm["cmsEnergy"].as<double>();
  hsPythiaConfig.processStrings = {vm["hsProcress"].as<std::string>()};
  auto hsPythiaGenerator        = std::make_shared<FW::Pythia8::TPythia8Generator>(
      hsPythiaConfig,
      Acts::getDefaultLogger("HardScatterTPythia8Generator",
                             logLevel));

  // create a pythia generator for the pile-up
  // MinBias with SD, DD and ND
  FW::Pythia8::TPythia8Generator::Config puPythiaConfig;
  puPythiaConfig.pdgBeam0       = vm["pdgBeam0"].as<int>();
  puPythiaConfig.pdgBeam1       = vm["pdgBeam1"].as<int>();
  puPythiaConfig.cmsEnergy      = vm["cmsEnergy"].as<double>();
  puPythiaConfig.processStrings = {vm["puProcress"].as<std::string>()};
  auto puPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      puPythiaConfig,
      Acts::getDefaultLogger("PileUpTPythia8Generator",
                             logLevel));
  // combined event generator
  FW::ReadEvgenAlgorithm::Config readEvgenCfg;
  readEvgenCfg.evgenCollection           = "EvgenParticles";
  readEvgenCfg.hardscatterEventReader    = hsPythiaGenerator;
  readEvgenCfg.pileupEventReader         = puPythiaGenerator;
  readEvgenCfg.randomNumbers             = random;
  readEvgenCfg.pileupPoissonParameter    = vm["pileup"].as<int>();
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
  if (bField.first && setupSimulation(sequencer, geom, random, bField.first, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  else if (setupSimulation(sequencer, geom, random, bField.second, logLevel) 
    != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;
  if (setupWriters(sequencer, barcode, outputDir) != FW::ProcessCode::SUCCESS)
    return EXIT_FAILURE;
  if (sequencer.run(nEvents) != FW::ProcessCode::SUCCESS) return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
