//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#include <stdio.h>
#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbers.hpp"
#include "RandomNumbersAlgorithm.hpp"

// the main hello world executable
int main (int argc, char *argv[]) {

    size_t nEvents = 1000;
    
    // Create the random number engine
    FW::RandomNumbers::Config brConfig;
    brConfig.gauss_parameters = {{ 0., 1. }};
    brConfig.uniform_parameters = {{ 0., 1. }};
    brConfig.landau_parameters = {{ 1., 7. }};
    brConfig.gamma_parameters = {{ 1., 1. }};
    
    std::shared_ptr<FW::RandomNumbers> bRandomNumbers(new FW::RandomNumbers(brConfig));
    
    // create the config object for the hello world algorithm
    FWE::RandomNumbersAlgorithm::Config rNumbersConfig;
    rNumbersConfig.randomNumbers = bRandomNumbers;
    rNumbersConfig.drawsPerEvent = 5000;
    // and now the hello world algorithm
    std::shared_ptr<FW::IAlgorithm> rNumbers(new FWE::RandomNumbersAlgorithm(rNumbersConfig));
    // create the config object for the sequencer
    FW::Sequencer::Config seqConfig;
    // now create the sequencer
    FW::Sequencer sequencer(seqConfig);
    sequencer.appendEventAlgorithms({rNumbers});
    
    // initialize loop
    sequencer.initializeEventLoop();
    // run the loop
    sequencer.processEventLoop(nEvents);
    // finalize loop
    sequencer.finalizeEventLoop();
    
}
