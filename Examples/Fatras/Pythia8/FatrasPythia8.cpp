/// @file
/// @brief Fatras example with a pythia8 generator and the generic detector

#include <cstdlib>
#include <boost/program_options.hpp>
#include <ACTS/Detector/TrackingGeometry.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Generator.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "FatrasCommon.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
      "events,n",
      po::value<size_t>()->default_value(10),
      "The number of events to be processed.")(
      "directory,d",
      po::value<std::string>()->default_value("."),
      "The output directory for the produced event data files.")(
      "loglevel,l",
      po::value<size_t>()->default_value(2),
      "The output log level.")(
      "cmsenergy,e",
      po::value<double>()->default_value(14000.),
      "The center of mass energy of the beam.")(
      "pileup,u",
      po::value<size_t>()->default_value(200),
      "The number of pileup events")(
      "bfieldmap,b",
      po::value<std::string>()->default_value(""),
      "The optional magnetic field map in root format and xyz grid.")(
      "seed,s",
      po::value<size_t>()->default_value(123456789),
      "The random number seed");
  
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  // help message messages
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // events
  size_t               nEvents   = 10;
  if (vm.count("events")) {
    nEvents =  vm["events"].as<size_t>();    
    std::cout << "- the number of events are set to " << nEvents << std::endl;
  } else {
    std::cout << "- default number of events are " << nEvents << std::endl;
  }
  // output directory
  std::string          outputDir = ".";
  if (vm.count("directory")) {
    outputDir =  vm["directory"].as<std::string>();    
    std::cout << "- the output directory is set to " << outputDir << std::endl;
  } else {
    std::cout << "- default output directory is " << outputDir << std::endl;
  }
  // output log level
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;  
  if (vm.count("loglevel")) {
    logLevel =   Acts::Logging::Level(vm["loglevel"].as<size_t>());    
    std::cout << "- the output log level is set to " << logLevel << std::endl;
  } else {
    std::cout << "- default log level is " << logLevel << std::endl;
  }
  // the cms energy 
  double cmsEnergy = 14000.;
  if (vm.count("cmsenergy")) {
    cmsEnergy = vm["cmsenergy"].as<double>();    
    std::cout << "- the beam cms energy is set to " << cmsEnergy << std::endl;
  } else {
    std::cout << "- default cms energy is " << cmsEnergy << std::endl;
  }  
  // the pileup level 
  size_t pileup = 200;  
  if (vm.count("pileup")) {
    pileup =  vm["pileup"].as<size_t>();    
    std::cout << "- the number of pileup is set to " << pileup << std::endl;
  } else {
    std::cout << "- default pileup is " << pileup << std::endl;
  }
  // seed for the random number
  size_t seed = 1234567890;
  if (vm.count("seed")) {
    pileup =  vm["seed"].as<size_t>();    
    std::cout << "- the random number seed is set to " << seed << std::endl;
  } else {
    std::cout << "- default random number seed is " << seed << std::endl;
  }
  // magnetic field map
  std::string fieldMap = "";
  if (vm.count("bfieldmap")) {
    fieldMap =  vm["bfieldmap"].as<std::string>();    
    std::cout << "- the magnetic field map is set to " << fieldMap << std::endl;
  } else {
    std::cout << "- default constant magnetic field. " << std::endl;
  }  
  
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
  hsPythiaConfig.cmsEnergy      = cmsEnergy;
  hsPythiaConfig.processStrings = {{"HardQCD:all = on"}};
  auto hsPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      hsPythiaConfig,
      Acts::getDefaultLogger("HardScatterTPythia8Generator", logLevel));
  // pythia8 soft scatter generator
  // MinBias with SD, DD and ND
  FW::Pythia8::TPythia8Generator::Config puPythiaConfig;
  puPythiaConfig.pdgBeam0       = 2212;
  puPythiaConfig.pdgBeam1       = 2212;
  puPythiaConfig.cmsEnergy      = cmsEnergy;
  puPythiaConfig.processStrings = {{"SoftQCD:all = on"}};
  auto puPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      puPythiaConfig,
      Acts::getDefaultLogger("PileUpTPythia8Generator", logLevel));
  // combined event generator
  FW::ReadEvgenAlgorithm::Config readEvgenCfg;
  readEvgenCfg.evgenCollection           = "EvgenParticles";
  readEvgenCfg.hardscatterEventReader    = hsPythiaGenerator;
  readEvgenCfg.pileupEventReader         = puPythiaGenerator;
  readEvgenCfg.randomNumbers             = random;
  readEvgenCfg.pileupPoissonParameter    = pileup;
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
