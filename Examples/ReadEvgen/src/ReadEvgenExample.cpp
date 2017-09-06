//
//  ReadEvgenExample.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>
#include <boost/program_options.hpp>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Generator.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"

namespace po = boost::program_options;

// the main read evgen executable
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,500,2);
  // add the evgen options
  FW::Options::addEvgenOptions<po::options_description>(desc);
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if needed
  // output messages
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options options
  auto standardOptions 
    = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents = standardOptions.first;
  auto logLevel = standardOptions.second;
  
  // create a pythia generator for the hard scatter
  // process: HardQCD for the moment
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

  // random number generation
  // Create the random number engine
  FW::RandomNumbersSvc::Config randomNumbersCfg;
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);

  // the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // get the read-in algorithm
  FW::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.evgenCollection = "EvgenParticles";
  // the hard scatter reader
  readEvgenCfg.hardscatterEventReader = hsPythiaGenerator;
  // the pileup reader
  readEvgenCfg.pileupEventReader = puPythiaGenerator;
  // the number of pileup events
  readEvgenCfg.randomNumbers          = randomNumbers;
  readEvgenCfg.pileupPoissonParameter = vm["pileup"].as<int>();
  readEvgenCfg.vertexTParameters      = {{0., 0.015}};
  readEvgenCfg.vertexZParameters      = {{0., 5.5}};
  // attach the barcode service
  readEvgenCfg.barcodeSvc = barcodeSvc;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FW::ReadEvgenAlgorithm>(
      readEvgenCfg,
      Acts::getDefaultLogger("ReadEvgenAlgorithm", logLevel));

  // Write ROOT TTree
  FW::Root::RootParticleWriter::Config particleWriterConfig;
  particleWriterConfig.collection = readEvgenCfg.evgenCollection;
  particleWriterConfig.filePath   = "EvgenParticles.root";
  particleWriterConfig.barcodeSvc = barcodeSvc;
  auto particleWriter
      = std::make_shared<FW::Root::RootParticleWriter>(particleWriterConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({randomNumbers});
  sequencer.addReaders({readEvgen});
  sequencer.appendEventAlgorithms({});
  sequencer.addWriters({particleWriter});
  sequencer.run(nEvents);
}
