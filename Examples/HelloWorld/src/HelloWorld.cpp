// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
#include <boost/program_options.hpp>
#include <cstdlib>
#include <memory>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "HelloWorldAlgorithm.hpp"

namespace po = boost::program_options;

// the main hello world executable
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
  // print help if reqested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);

  // and now the hello world algorithm
  std::shared_ptr<FW::IAlgorithm> hWorld(
      new FWE::HelloWorldAlgorithm(standardOptions.second));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.appendEventAlgorithms({hWorld});
  sequencer.run(standardOptions.first);
}
