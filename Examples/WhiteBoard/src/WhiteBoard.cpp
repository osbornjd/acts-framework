// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Framework/WhiteBoard.hpp"

#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "WhiteBoardAlgorithm.hpp"

int
main(int argc, char* argv[])
{
  // setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) { return EXIT_FAILURE; }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  auto logLevel = FW::Options::readLogLevel(vm);

  // Create an algorithm that writes to the event store
  FW::WhiteBoardAlgorithm::Config wBoardConfigWrite;
  wBoardConfigWrite.outputClassOneCollection = "ClassOneCollection";
  wBoardConfigWrite.outputClassTwoCollection = "ClassTwoCollection";
  sequencer.addAlgorithm(
      std::make_shared<FW::WhiteBoardAlgorithm>(wBoardConfigWrite, logLevel));

  // Create an algorithm that reads from the event store
  FW::WhiteBoardAlgorithm::Config wBoardConfigRead;
  wBoardConfigRead.inputClassOneCollection = "ClassOneCollection";
  wBoardConfigRead.inputClassTwoCollection = "ClassTwoCollection";
  sequencer.addAlgorithm(
      std::make_shared<FW::WhiteBoardAlgorithm>(wBoardConfigRead, logLevel));

  return sequencer.run();
}
