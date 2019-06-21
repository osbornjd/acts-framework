// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "HelloWorldAlgorithm.hpp"

/// Main read evgen executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
  // setup and parse options
  auto opt = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(opt);
  auto vm = FW::Options::parse(opt, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  auto logLevel = FW::Options::readLogLevel(vm);

  // add w/ HelloWorld algorithm
  sequencer.addAlgorithm(std::make_shared<FW::HelloWorldAlgorithm>(logLevel));

  return sequencer.run();
}
