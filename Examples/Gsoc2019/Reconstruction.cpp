// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cstdlib>
#include <memory>

#include <boost/program_options.hpp>

#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Reconstruction/Empty.hpp"

namespace po = boost::program_options;

using namespace FW;

int
main(int argc, char* argv[])
{
  // Setup command line arguments and options
  po::options_description desc("Gsoc2019 reconstruction tool");
  // Add common options, i.e. number of events, ...
  Options::addCommonOptions(desc);

  // Process command line arguments and options
  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // Print help if reqested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }

  // extract common options
  auto nEvents  = Options::readNumberOfEvents(vm);
  auto logLevel = Options::readLogLevel(vm);

  // Create a sequencer w/ the default config
  Sequencer sequencer({});

  // Add the empty reconstruction algorithm
  EmptyReconstructionAlgorithm::Config emptyReco;
  sequencer.appendEventAlgorithms(
      {std::make_shared<EmptyReconstructionAlgorithm>(emptyReco, logLevel)});

  return (sequencer.run(nEvents) == ProcessCode::SUCCESS) ? EXIT_SUCCESS
                                                          : EXIT_FAILURE;
}
