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
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "RandomNumbersAlgorithm.hpp"

int
main(int argc, char* argv[])
{
  // setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addRandomNumbersOptions(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  auto logLevel = FW::Options::readLogLevel(vm);

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // Create the random number engine
  auto randomNumbersCfg = FW::Options::readRandomNumbersConfig(vm);
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);

  // Create the config object for the hello world algorithm
  FW::RandomNumbersAlgorithm::Config rNumbersConfig;
  rNumbersConfig.randomNumbers     = randomNumbers;
  rNumbersConfig.gaussParameters   = {{0., 1.}};
  rNumbersConfig.uniformParameters = {{0., 1.}};
  rNumbersConfig.gammaParameters   = {{1., 1.}};
  rNumbersConfig.drawsPerEvent     = 5000;
  sequencer.addAlgorithm(
      std::make_shared<FW::RandomNumbersAlgorithm>(rNumbersConfig, logLevel));

  return sequencer.run();
}
