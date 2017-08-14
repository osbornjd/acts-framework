//
//  ReadEvgenExample.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Plugins/Pythia8/TPythia8Generator.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 500;

  // create a pythia generator for the hard scatter
  // process: HardQCD for the moment
  FW::Pythia8::TPythia8Generator::Config hsPythiaConfig;
  hsPythiaConfig.pdgBeam0       = 2212;
  hsPythiaConfig.pdgBeam1       = 2212;
  hsPythiaConfig.cmsEnergy      = 14000.;
  hsPythiaConfig.processStrings = {{"HardQCD:all = on"}};
  auto hsPythiaGenerator        = std::make_shared<FW::Pythia8::TPythia8Generator>(
      hsPythiaConfig,
      Acts::getDefaultLogger("HardScatterTPythia8Generator",
                             Acts::Logging::VERBOSE));

  // create a pythia generator for the pile-up
  // MinBias with SD, DD and ND
  FW::Pythia8::TPythia8Generator::Config puPythiaConfig;
  puPythiaConfig.pdgBeam0       = 2212;
  puPythiaConfig.pdgBeam1       = 2212;
  puPythiaConfig.cmsEnergy      = 14000.;
  puPythiaConfig.processStrings = {{"SoftQCD:all = on"}};
  auto puPythiaGenerator = std::make_shared<FW::Pythia8::TPythia8Generator>(
      puPythiaConfig,
      Acts::getDefaultLogger("PileUpTPythia8Generator",
                             Acts::Logging::VERBOSE));

  // random number generation
  // Create the random number engine
  FW::RandomNumbersSvc::Config randomNumbersCfg;
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);

  // the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", Acts::Logging::INFO));

  // get the read-in algorithm
  FW::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.particlesCollection = "EvgenParticles";
  // the hard scatter reader
  readEvgenCfg.hardscatterParticleReader = hsPythiaGenerator;
  // the pileup reader
  readEvgenCfg.pileupParticleReader = puPythiaGenerator;
  // the number of pileup eventst
  readEvgenCfg.randomNumbers          = randomNumbers;
  readEvgenCfg.pileupPoissonParameter = 200;
  readEvgenCfg.vertexTParameters      = {{0., 0.015}};
  readEvgenCfg.vertexZParameters      = {{0., 5.5}};
  // attach the barcode service
  readEvgenCfg.barcodeSvc = barcodeSvc;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FW::ReadEvgenAlgorithm>(
      readEvgenCfg,
      Acts::getDefaultLogger("ReadEvgenAlgorithm", Acts::Logging::INFO));

  // Write ROOT TTree
  FW::Root::RootParticleWriter::Config particleWriterConfig;
  particleWriterConfig.collection = readEvgenCfg.particlesCollection;
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
