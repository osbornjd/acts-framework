//
//  ReadEvgenExample.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/RootPythia8/ParticleGenerator.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 100;

  // creating the data stores
  Acts::Logging::Level eventStoreLogLevel = Acts::Logging::INFO;
  auto                 detectorStore      = std::make_shared<FW::WhiteBoard>(
      Acts::getDefaultLogger("DetectorStore", Acts::Logging::INFO));

  // create a pythi generator
  FWRootPythia8::ParticleGenerator::Config pythiaConfig;
  auto                                     pythiaGenerator
      = std::make_shared<FWRootPythia8::ParticleGenerator>(
        pythiaConfig,
  Acts::getDefaultLogger("ParticleGenerator", Acts::Logging::VERBOSE));

  // random number generation
  // Create the random number engine
  FW::RandomNumbersSvc::Config pileupNumbersCfg;
  pileupNumbersCfg.poisson_parameter = 40;
  std::shared_ptr<FW::RandomNumbersSvc> pileupNumbers(
      new FW::RandomNumbersSvc(pileupNumbersCfg));

  FW::RandomNumbersSvc::Config pileupVertexTCfg;
  pileupVertexTCfg.gauss_parameters = {{0., 0.015}};
  std::shared_ptr<FW::RandomNumbersSvc> pileupVertexT(
      new FW::RandomNumbersSvc(pileupVertexTCfg));

  FW::RandomNumbersSvc::Config pileupVertexZCfg;
  pileupVertexZCfg.gauss_parameters = {{0., 5.5}};
  std::shared_ptr<FW::RandomNumbersSvc> pileupVertexZ(
      new FW::RandomNumbersSvc(pileupVertexZCfg));

  // get the read-in algorithm
  FWE::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.particleCollectionName = "SimulationParticles";
  /// the hard scatter reader
  readEvgenCfg.hardscatterParticleReader = pythiaGenerator;
  /// the pileup reader
  readEvgenCfg.pileupParticleReader = pythiaGenerator;
  /// the number of pileup events
  readEvgenCfg.pileupType          = FW::Distribution::poisson;
  readEvgenCfg.pileupRandomNumbers = pileupNumbers;
  readEvgenCfg.pileupVertexDistT   = pileupVertexT;
  readEvgenCfg.pileupVertexDistZ   = pileupVertexZ;
  /// the job WhiteBoard
  readEvgenCfg.jBoard = detectorStore;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FWE::ReadEvgenAlgorithm>
    (readEvgenCfg, Acts::getDefaultLogger("ReadEvgenAlgorithm", Acts::Logging::VERBOSE));
    

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({pileupNumbers, pileupVertexT, pileupVertexZ});
  sequencer.addIOAlgorithms({readEvgen});
  sequencer.appendEventAlgorithms({});

  // initialize loop
  sequencer.initializeEventLoop();
  // run the loop
  sequencer.processEventLoop(nEvents);
  // finalize loop
  sequencer.finalizeEventLoop();
}

