// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_BFIELD_BFIELDEXAMPLE_H
#define ACTFW_BFIELD_BFIELDEXAMPLE_H

#include <boost/program_options.hpp>
#include <string>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/RootInterpolatedBFieldWriter.hpp"
#include "TROOT.h"

/// The main executable
///
/// Creates an InterpolatedBFieldMap from a txt or csv file and writes out the
/// grid points and values of the map into root format. The Field can then be
/// displayed using the root script printBField.cpp

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // enable root thread safety in order to use root writers in multi threaded
  // mode
  ROOT::EnableThreadSafety();

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add an output file
  desc.add_options()("bf-file-out",
                     po::value<std::string>()->default_value("BFieldOut.root"),
                     "Set this name for an output root file.")(
      "bf-map-out",
      po::value<std::string>()->default_value("bField"),
      "Set this name for the tree in the out file.");

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
  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  if (!bField.first) {
    std::cout << "Bfield could not be set up. Exiting." << std::endl;
    return -1;
  }

  // Write the interpolated magnetic field
  FW::BField::RootInterpolatedBFieldWriter::Config writerConfig;
  if (vm["bf-rz"].as<bool>())
    writerConfig.gridType = FW::BField::GridType::rz;
  else
    writerConfig.gridType = FW::BField::GridType::xyz;
  writerConfig.treeName   = vm["bf-map-out"].as<std::string>();
  writerConfig.fileName   = vm["bf-file-out"].as<std::string>();

  writerConfig.bField = bField.first;
  FW::BField::RootInterpolatedBFieldWriter::run(writerConfig);
}

#endif  // ACTFW_BFIELD_BFIELDEXAMPLE_H
