// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <string>
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "detail/BFieldWritingBase.hpp"

/// The main executable
///
/// Creates an InterpolatedBFieldMap from a txt or csv file and writes out the
/// grid points and values of the map into root format. The Field can then be
/// displayed using the root script printBField.cpp

namespace po = boost::program_options;

/// @brief main executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // Add an output file
  desc.add_options()("bf-file-out",
                     po::value<std::string>()->default_value("BFieldOut.root"),
                     "Set this name for an output root file.")(
      "bf-map-out",
      po::value<std::string>()->default_value("bField"),
      "Set this name for the tree in the out file.")(
      "bf-out-rz",
      po::value<bool>()->default_value(false),
      "Please set this flag to true, if you want to print out the field map in "
      "cylinder coordinates (r,z). The default are cartesian coordinates "
      "(x,y,z). ")(
      "bf-rRange",
      po::value<read_range>()->multitoken(),
      "[optional] range which the bfield map should be written out in either r "
      "(cylinder "
      "coordinates) or x/y (cartesian coordinates)  in [mm]. In case no value "
      "is handed over the whole map will be written out. Please "
      "hand over by simply seperating the values by space")(
      "bf-zRange",
      po::value<read_range>()->multitoken(),
      "[optional] range which the bfield map should be written out in z in "
      "[mm].In case no value is handed over for 'bf-rRange' and 'bf-zRange the "
      "whole map will be written out. "
      "Please hand over by simply seperating the values by space")(
      "bf-rBins",
      po::value<size_t>()->default_value(200),
      "[optional] The number of bins in r. This parameter only needs to be "
      "specified if 'bf-rRange' and 'bf-zRange' are given.")(
      "bf-ZBins",
      po::value<size_t>()->default_value(300),
      "[optional] The number of bins in z. This parameter only needs to be "
      "specified if 'bf-rRange' and 'bf-zRange' are given.")(
      "bf-PhiBins",
      po::value<size_t>()->default_value(100),
      "[optional] The number of bins in phi. This parameter only needs to be "
      "specified if 'bf-rRange' and 'bf-zRange' are given and 'bf-out-rz' is "
      "turned on.");

  // Map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  // Print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);

  auto field2D = std::get<std::shared_ptr<InterpolatedBFieldMap2D>>(bField);
  auto field3D = std::get<std::shared_ptr<InterpolatedBFieldMap3D>>(bField);

  if (!field2D && !field3D) {
    std::cout << "Bfield map could not be read. Exiting." << std::endl;
    return -1;
  }

  // write it out
  if (field2D) {
    // the 2-dimensional field case
    FW::BField::writeField<po::variables_map, InterpolatedBFieldMap2D>(vm,
                                                                       field2D);
  } else {
    // the 3-dimensional field case
    FW::BField::writeField<po::variables_map, InterpolatedBFieldMap3D>(vm,
                                                                       field3D);
  }

  // Return 0 for success
  return 0;
}
