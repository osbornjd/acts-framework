// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/GeometryOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Obj/ObjExCellWriter.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Utilities/Paths.hpp"

namespace po = boost::program_options;

/// @brief The Extrapolation example
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param argv the argument list
template <typename geometry_options_t, typename geometry_getter_t>
int
extrapolationExample(int                argc,
                     char*              argv[],
                     geometry_options_t geometryOptions,
                     geometry_getter_t  trackingGeometry)
{

  // Create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // Now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the standard options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // Add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // Add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // Add the fatras options
  FW::Options::addExtrapolationOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);

  // Add specific options for this geometry
  geometryOptions(desc);

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
  // Now read the common options
  auto nEvents  = FW::Options::readNumberOfEvents<po::variables_map>(vm);
  auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);

  // Add it to the sequencer
  sequencer.addServices({randomNumberSvc});

  // Get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // Create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);

  std::shared_ptr<FW::ExtrapolationAlgorithm> extrapolationAlg = nullptr;

  // Charged extrapolator in case we have a interpolated magnetic field present
  if (bField.first) {
    // Resolve the bfield map template and create the algorithm
    auto exAlgConfig
        = FW::Options::readExtrapolationConfig(vm, bField.first, tGeometry);
    exAlgConfig.randomNumberSvc = randomNumberSvc;
    extrapolationAlg
        = std::make_shared<FW::ExtrapolationAlgorithm>(exAlgConfig, logLevel);
  } else {
    // Charged extrapolator in case we have a constant magnetic field
    auto exAlgConfig
        = FW::Options::readExtrapolationConfig(vm, bField.second, tGeometry);
    exAlgConfig.randomNumberSvc = randomNumberSvc;
    extrapolationAlg
        = std::make_shared<FW::ExtrapolationAlgorithm>(exAlgConfig, logLevel);
  }

  // Append the algorithm
  sequencer.appendEventAlgorithms({extrapolationAlg});

  // ---------------------------------------------------------------------------------
  // Output directory
  std::string outputDir = vm["output-dir"].template as<std::string>();
  auto        ecc       = vm["ext-charged-cells"].as<std::string>();
  auto        ecn       = vm["ext-neutral-cells"].as<std::string>();

  // Write ROOT TTree(s)
  if (vm["output-root"].as<bool>()) {

    // Ec for charged particles
    FW::Root::RootExCellWriter<Acts::TrackParameters>::Config
        eccWriterRootConfig;
    eccWriterRootConfig.filePath      = FW::joinPaths(outputDir, ecc + ".root");
    eccWriterRootConfig.treeName      = ecc;
    eccWriterRootConfig.collection    = ecc;
    eccWriterRootConfig.writeBoundary = false;
    eccWriterRootConfig.writeMaterial = true;
    eccWriterRootConfig.writeSensitive = true;
    eccWriterRootConfig.writePassive   = true;
    auto eccWriterRoot
        = std::make_shared<FW::Root::RootExCellWriter<Acts::TrackParameters>>(
            eccWriterRootConfig);

    // Ec for neutral particles
    FW::Root::RootExCellWriter<Acts::NeutralParameters>::Config
        ecnWriterRootConfig;
    ecnWriterRootConfig.filePath      = FW::joinPaths(outputDir, ecn + ".root");
    ecnWriterRootConfig.treeName      = ecn;
    ecnWriterRootConfig.collection    = ecn;
    ecnWriterRootConfig.writeBoundary = false;
    ecnWriterRootConfig.writeMaterial = true;
    ecnWriterRootConfig.writeSensitive = true;
    ecnWriterRootConfig.writePassive   = true;
    auto ecnWriterRoot
        = std::make_shared<FW::Root::RootExCellWriter<Acts::NeutralParameters>>(
            ecnWriterRootConfig);
    // Add the writers
    sequencer.addWriters({eccWriterRoot, ecnWriterRoot});
  }

  // Write OBJ Files
  if (vm["output-obj"].as<bool>()) {

    // Ec for charged particles
    FW::Obj::ObjExCellWriter<Acts::TrackParameters>::Config eccWriterObjConfig;
    eccWriterObjConfig.collection = ecc;
    eccWriterObjConfig.outputDir  = outputDir;
    auto eccWriterObj
        = std::make_shared<FW::Obj::ObjExCellWriter<Acts::TrackParameters>>(
            eccWriterObjConfig);

    // Ec for neutral particles
    FW::Obj::ObjExCellWriter<Acts::NeutralParameters>::Config
        ecnWriterObjConfig;
    ecnWriterObjConfig.collection = ecn;
    ecnWriterObjConfig.outputDir  = outputDir;
    auto ecnWriterObj
        = std::make_shared<FW::Obj::ObjExCellWriter<Acts::NeutralParameters>>(
            ecnWriterObjConfig);

    // Add the writers
    sequencer.addWriters({eccWriterObj, ecnWriterObj});
  }

  // Initiate the run
  sequencer.run(nEvents);
  // Return 0 for success
  return 0;
}
