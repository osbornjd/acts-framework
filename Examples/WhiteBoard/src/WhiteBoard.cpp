// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//
//  WhiteBoard.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include "ACTFW/Framework/WhiteBoard.hpp"
#include <boost/program_options.hpp>
#include <cstdlib>
#include <memory>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "WhiteBoardAlgorithm.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 10, 2);
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents  = standardOptions.first;
  auto logLevel = standardOptions.second;

  // Create an algorithm that writes to the event store
  FWE::WhiteBoardAlgorithm::Config wBoardConfigWrite;
  wBoardConfigWrite.outputClassOneCollection = "ClassOneCollection";
  wBoardConfigWrite.outputClassTwoCollection = "ClassTwoCollection";
  auto wBoardWrite
      = std::make_shared<FWE::WhiteBoardAlgorithm>(wBoardConfigWrite, logLevel);

  // Create an algorithm that reads from the event store
  FWE::WhiteBoardAlgorithm::Config wBoardConfigRead;
  wBoardConfigRead.inputClassOneCollection = "ClassOneCollection";
  wBoardConfigRead.inputClassTwoCollection = "ClassTwoCollection";
  auto wBoardRead
      = std::make_shared<FWE::WhiteBoardAlgorithm>(wBoardConfigRead, logLevel);

  // Create the event loop
  FW::Sequencer::Config seqConfig;
  seqConfig.eventStoreLogLevel = logLevel;
  FW::Sequencer sequencer(seqConfig);
  sequencer.appendEventAlgorithms({wBoardWrite, wBoardRead});

  // run the event loop
  sequencer.run(nEvents);
}
