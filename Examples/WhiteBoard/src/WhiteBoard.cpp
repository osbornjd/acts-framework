//
//  WhiteBoard.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include <stdio.h>
#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "WhiteBoardAlgorithm.hpp"

// the main hello world executable
int main (int argc, char *argv[]) {

    size_t nEvents = 10;
    
    // Creating the EventStore
    FW::WhiteBoard::Config eventStoreConfig("EventStore");
    // and now the hello world algorithm
    std::shared_ptr<FW::WhiteBoard> eventStore(new FW::WhiteBoard(eventStoreConfig));
    
    // Creating the DetectorStore
    FW::WhiteBoard::Config detectorStoreConfig("DetectorStore");
    // and now the hello world algorithm
    std::shared_ptr<FW::WhiteBoard> detectorStore(new FW::WhiteBoard(detectorStoreConfig));
    
    // create the config object for the hello world algorithm
    FWE::WhiteBoardAlgorithm::Config wBoardConfigWrite("WriteAlgorithm");
    wBoardConfigWrite.outputClassOneCollection = "ClassOneCollection";
    wBoardConfigWrite.outputClassTwoCollection = "ClassTwoCollection";

    // and now the hello world algorithm
    std::shared_ptr<FW::IAlgorithm> wBoardWrite(new FWE::WhiteBoardAlgorithm(wBoardConfigWrite));

    // create the config object for the hello world algorithm
    FWE::WhiteBoardAlgorithm::Config wBoardConfigRead("ReadAlgorithm");
    wBoardConfigRead.inputClassOneCollection  = "ClassOneCollection";
    wBoardConfigRead.inputClassTwoCollection  = "ClassTwoCollection";
    // and now the hello world algorithm
    std::shared_ptr<FW::IAlgorithm> wBoardRead(new FWE::WhiteBoardAlgorithm(wBoardConfigRead));
    
    // create the config object for the sequencer
    FW::Sequencer::Config seqConfig("WhiteBoardSeqeuncer");
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
