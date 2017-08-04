//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

#include "RandomNumbersAlgorithm.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 1000;

  // Create the random number engine
  FW::RandomNumbersSvc::Config          brConfig;
  std::shared_ptr<FW::RandomNumbersSvc> bRandomNumbers(
      new FW::RandomNumbersSvc(brConfig));

  // create the config object for the hello world algorithm
  FWE::RandomNumbersAlgorithm::Config rNumbersConfig;
  rNumbersConfig.randomNumbers     = bRandomNumbers;
  rNumbersConfig.gaussParameters   = {{0., 1.}};
  rNumbersConfig.uniformParameters = {{0., 1.}};
  rNumbersConfig.landauParameters  = {{1., 7.}};
  rNumbersConfig.gammaParameters   = {{1., 1.}};
  rNumbersConfig.drawsPerEvent     = 5000;
  // and now the hello world algorithm
  std::shared_ptr<FW::IAlgorithm> rNumbers(
      new FWE::RandomNumbersAlgorithm(rNumbersConfig));
  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({bRandomNumbers});
  sequencer.appendEventAlgorithms({rNumbers});
  sequencer.run(nEvents);
}
