//
//  WhiteBoard.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "WhiteBoardAlgorithm.hpp"

// the main hello world executable
int main (int argc, char *argv[]) {

    size_t nEvents = 10;

    // creating the data stores
    auto eventStore = std::make_shared<FW::WhiteBoard>(Acts::getDefaultLogger("EventStore", Acts::Logging::INFO));
    auto detectorStore = std::make_shared<FW::WhiteBoard>(Acts::getDefaultLogger("DetectorStore", Acts::Logging::INFO));
    
    // create the config object for the hello world algorithm
    FWE::WhiteBoardAlgorithm::Config wBoardConfigWrite;
    wBoardConfigWrite.outputClassOneCollection = "ClassOneCollection";
    wBoardConfigWrite.outputClassTwoCollection = "ClassTwoCollection";

    // and now the hello world algorithm
    std::shared_ptr<FW::IAlgorithm> wBoardWrite(new FWE::WhiteBoardAlgorithm(wBoardConfigWrite));

    // create the config object for the hello world algorithm
    FWE::WhiteBoardAlgorithm::Config wBoardConfigRead;
    wBoardConfigRead.inputClassOneCollection  = "ClassOneCollection";
    wBoardConfigRead.inputClassTwoCollection  = "ClassTwoCollection";
    // and now the hello world algorithm
    std::shared_ptr<FW::IAlgorithm> wBoardRead(new FWE::WhiteBoardAlgorithm(wBoardConfigRead));
    
    // create the config object for the sequencer
    FW::Sequencer::Config seqConfig;
    seqConfig.eventBoard = eventStore;
    seqConfig.jobBoard   = detectorStore;
    
    // now create the sequencer
    FW::Sequencer sequencer(seqConfig);
    sequencer.appendEventAlgorithms({wBoardWrite,wBoardRead});
    
    // initialize loop
    sequencer.initializeEventLoop();
    // run the loop
    sequencer.processEventLoop(nEvents);
    // finalize loop
    sequencer.finalizeEventLoop();
    
}
