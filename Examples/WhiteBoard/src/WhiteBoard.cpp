//
//  WhiteBoard.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

#include "WhiteBoardAlgorithm.hpp"

int
main(int argc, char* argv[])
{
  size_t nEvents = 10;

  // Create an algorithm that writes to the event store
  FWE::WhiteBoardAlgorithm::Config wBoardConfigWrite;
  wBoardConfigWrite.outputClassOneCollection = "ClassOneCollection";
  wBoardConfigWrite.outputClassTwoCollection = "ClassTwoCollection";
  auto wBoardWrite = std::make_shared<FWE::WhiteBoardAlgorithm>(wBoardConfigWrite);

  // Create an algorithm that reads from the event store
  FWE::WhiteBoardAlgorithm::Config wBoardConfigRead;
  wBoardConfigRead.inputClassOneCollection = "ClassOneCollection";
  wBoardConfigRead.inputClassTwoCollection = "ClassTwoCollection";
  auto wBoardRead = std::make_shared<FWE::WhiteBoardAlgorithm>(wBoardConfigRead);

  // Create the event loop
  FW::Sequencer::Config seqConfig;
  seqConfig.eventStoreLogLevel = Acts::Logging::VERBOSE;
  FW::Sequencer sequencer(seqConfig);
  sequencer.appendEventAlgorithms({wBoardWrite, wBoardRead});

  // run the event loop
  sequencer.run(nEvents);
}
