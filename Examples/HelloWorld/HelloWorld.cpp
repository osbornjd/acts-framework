// This file is part of the Acts project.
//
// Copyright (C) 2017,2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief An example tools that shows the sequencer functionality

#include <cstdlib>
#include <memory>

#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Options/CommonOptions.hpp"

#include "HelloWorldAlgorithm.hpp"

int
main(int argc, char* argv[])
{
  // setup options
  // every component should have an associated option setup function
  // that should be called here.
  auto opt = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(opt);
  FW::Options::addRandomNumbersOptions(opt);
  // parse options from command line flags
  auto vm = FW::Options::parse(opt, argc, argv);
  // an empty varaibles map indicates an error
  if (vm.empty()) { return EXIT_FAILURE; }

  // extract some common options
  auto logLevel = FW::Options::readLogLevel(vm);

  // setup basic tools shared among algorithms
  auto rnd = std::make_shared<FW::RandomNumbers>(
      FW::Options::readRandomNumbersConfig(vm));

  // setup the sequencer first w/ config derived from options
  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // add HelloWorld algorithm that does nothing
  sequencer.addAlgorithm(std::make_shared<FW::HelloWorldAlgorithm>(logLevel));

  return sequencer.run();
}
