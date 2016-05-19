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

#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Examples/GenericDetectorExample/BuildGenericDetector.hpp"

#include "ExtrapolationTestAlgorithm.hpp"


// the main hello world executable
int main (int argc, char *argv[]) {

    size_t nEvents = 10000;

    // create the tracking geometry as a shared pointer
    std::unique_ptr<const Acts::TrackingGeometry> tGeometry = Acts::trackingGeometry();
    
    //
    
    // create the config object for the sequencer
    FW::Sequencer::Config seqConfig;
    seqConfig.name = "ExtrapolationTestSequencer";
    // now create the sequencer
    FW::Sequencer sequencer(seqConfig);
    sequencer.appendEventAlgorithms({});
    
    // initialize loop
    sequencer.initializeEventLoop();
    // run the loop
    sequencer.processEventLoop(nEvents);
    // finalize loop
    sequencer.finalizeEventLoop();
    
}
