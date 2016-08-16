//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "HelloWorldAlgorithm.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 10;

  // create the config object for the hello world algorithm
  FW::Algorithm::Config hWorldConfig;
  // and now the hello world algorithm
  std::shared_ptr<FW::IAlgorithm> hWorld(
      new FWE::HelloWorldAlgorithm(hWorldConfig));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.appendEventAlgorithms({hWorld});

  // initialize loop
  sequencer.initializeEventLoop();
  // run the loop
  sequencer.processEventLoop(nEvents);
  // finalize loop
  sequencer.finalizeEventLoop();
}
