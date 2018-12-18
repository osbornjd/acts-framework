// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/GeometryOptions.hpp"
#include "ACTFW/Common/InputOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/MaterialMapping/MaterialMapping.hpp"
#include "ACTFW/Plugins/Root/RootIndexedMaterialWriter.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackReader.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/Plugins/MaterialMapping/SurfaceMaterialMapper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

namespace po = boost::program_options;

/// @brief The material validation example, it runs a propagation
/// and then writes out the material information
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param atgv the argument list
/// @param trackingGeometry is the access struct for the trackingGeometry
///
template <typename geometry_options_t, typename geometry_getter_t>
int
materialMappingExample(int                argc,
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
  // Add the common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addInputOptions<po::options_description>(desc);
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

  // The Log level
  auto nEvents  = FW::Options::readNumberOfEvents<po::variables_map>(vm);
  auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

  // Get the tracking geometry and setup the propagator
  auto tGeometry = trackingGeometry(vm);
  // Get a Navigator
  Acts::Navigator navigator(tGeometry);

  // Straight line stepper
  using SlStepper  = Acts::StraightLineStepper;
  using Propagator = Acts::Propagator<SlStepper, Acts::Navigator>;
  // Make stepper and propagator
  SlStepper  stepper;
  Propagator propagator(std::move(stepper), std::move(navigator));

  auto matCollection = vm["input-collection"].template as<std::string>();

  // ---------------------------------------------------------------------------------
  // Input directory & input file handling
  std::string intputDir   = vm["input-dir"].template as<std::string>();
  std::string intputFiles = vm["input-files"].template as<std::string>();

  if (vm["input-root"].template as<bool>()) {
    // Read the material step information from a ROOT TTree
    FW::Root::RootMaterialTrackReader::Config matTrackReaderRootConfig;
    if (not matCollection.empty()) {
      matTrackReaderRootConfig.collection = matCollection;
    }
    matTrackReaderRootConfig.fileList = FW::splitFiles(intputFiles, ',');
    auto matTrackReaderRoot
        = std::make_shared<FW::Root::RootMaterialTrackReader>(
            matTrackReaderRootConfig);
    if (sequencer.addReaders({matTrackReaderRoot}) != FW::ProcessCode::SUCCESS)
      return -1;
  }

  /// The material mapper
  Acts::SurfaceMaterialMapper::Config smmConfig;
  auto smm = std::make_shared<Acts::SurfaceMaterialMapper>(
      smmConfig,
      std::move(propagator),
      Acts::getDefaultLogger("SurfaceMaterialMapper", logLevel));

  /// The writer of the indexed material
  FW::Root::RootIndexedMaterialWriter::Config rimConfig(
      "IndexedMaterialWriter");
  auto rimRootWriter
      = std::make_shared<FW::Root::RootIndexedMaterialWriter>(rimConfig);

  /// The material mapping algorithm
  FW::MaterialMapping::Config mmAlgConfig;
  mmAlgConfig.materialMapper        = smm;
  mmAlgConfig.trackingGeometry      = tGeometry;
  mmAlgConfig.indexedMaterialWriter = rimRootWriter;

  // Create the material mapping
  auto mmAlg = std::make_shared<FW::MaterialMapping>(mmAlgConfig);

  // Apend the Algorithm
  sequencer.appendEventAlgorithms({mmAlg});

  // Initiate the run
  sequencer.run(nEvents);
  // Return success code
  return 0;
}
