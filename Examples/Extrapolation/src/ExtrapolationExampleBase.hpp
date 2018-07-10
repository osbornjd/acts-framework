// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Geometry/GeometryOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

namespace po = boost::program_options;

/// The Extrapolation example
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param atgv the argument list
///
template <typename geometry_options_t, typename geometry_getter_t>
int
extrapolationExample(int                argc,
                     char*              argv[],
                     geometry_options_t geometryOptions,
                     geometry_getter_t  trackingGeometry)
{

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the fatras options
  FW::Options::addExtrapolationOptions<po::options_description>(desc);

  // add specific options for this geometry
  geometryOptions(desc);

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

  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);
  // add it to the sequencer
  sequencer.addServices({randomNumberSvc});

  // get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);

  std::shared_ptr<FW::ExtrapolationAlgorithm> extrapolationAlg = nullptr;
  // a charged propagator
  if (bField.first) {
    // resolve the bfield map template and create the algorithm
    auto exAlgConfig
        = FW::Options::readExtrapolationConfig(vm, bField.first, tGeometry);
    exAlgConfig.randomNumberSvc = randomNumberSvc;
    extrapolationAlg
        = std::make_shared<FW::ExtrapolationAlgorithm>(exAlgConfig, logLevel);
  } else {
    // resolve the bfield map template and create the algorithm
    auto exAlgConfig
        = FW::Options::readExtrapolationConfig(vm, bField.second, tGeometry);
    exAlgConfig.randomNumberSvc = randomNumberSvc;
    extrapolationAlg
        = std::make_shared<FW::ExtrapolationAlgorithm>(exAlgConfig, logLevel);
  }

  // Write ROOT TTree
  // ecc for charged particles
  FW::Root::RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.filePath       = "excells_charged.root";
  reccWriterConfig.treeName       = "extrapolation_charged";
  reccWriterConfig.collection     = vm["ext-charged-cells"].as<std::string>();
  reccWriterConfig.writeBoundary  = false;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  auto rootEccWriter
      = std::make_shared<FW::Root::RootExCellWriter<Acts::TrackParameters>>(
          reccWriterConfig);

  // ecc for neutral particles
  FW::Root::RootExCellWriter<Acts::NeutralParameters>::Config recnWriterConfig;
  recnWriterConfig.filePath       = "excells_neutral.root";
  recnWriterConfig.treeName       = "extrapolation_neutral";
  recnWriterConfig.collection     = vm["ext-neutral-cells"].as<std::string>();
  recnWriterConfig.writeBoundary  = false;
  recnWriterConfig.writeMaterial  = true;
  recnWriterConfig.writeSensitive = true;
  recnWriterConfig.writePassive   = true;
  auto rootEcnWriter
      = std::make_shared<FW::Root::RootExCellWriter<Acts::NeutralParameters>>(
          recnWriterConfig);
  // add the writers
  sequencer.addWriters({rootEccWriter, rootEcnWriter});
  // append the algorithm
  sequencer.appendEventAlgorithms({extrapolationAlg});
  // initiate the run
  sequencer.run(nEvents);
  return 1;
}
