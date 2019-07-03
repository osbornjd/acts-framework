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
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Geometry/CommonGeometry.hpp"
#include "ACTFW/MaterialMapping/MaterialMapping.hpp"
#include "ACTFW/Plugins/Json/JsonGeometryConverter.hpp"
#include "ACTFW/Plugins/Json/JsonMaterialWriter.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackReader.hpp"
#include "ACTFW/Plugins/Root/RootMaterialWriter.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTFW/options/CommonOptions.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/Plugins/MaterialMapping/SurfaceMaterialMapper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

namespace po = boost::program_options;

/// @brief The material validation example, it runs a propagation
/// and then writes out the material information
///
/// @tparam option_setup_t Type of the option setter
/// @tparam geometry_setup_t Type of the geometry setter
///
/// @param argc the number of argumetns of the call
/// @param atgv the argument list
/// @param optionsSetup is the access struct to the additional options
/// @param geometrySetup is the access struct for the trackingGeometry
///
template <typename options_setup_t, typename geometry_setup_t>
int
materialMappingExample(int              argc,
                       char*            argv[],
                       options_setup_t  optionsSetup,
                       geometry_setup_t geometrySetup)
{

  // Setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addGeometryOptions(desc);
  FW::Options::addMaterialOptions(desc);
  FW::Options::addBFieldOptions(desc);
  FW::Options::addRandomNumbersOptions(desc);
  FW::Options::addPropagationOptions(desc);
  FW::Options::addOutputOptions(desc);

  // Add specific options for this geometry
  optionsSetup(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // Get the log level
  auto logLevel = FW::Options::readLogLevel(vm);

  // The geometry, material and decoration
  auto geometry  = FW::Geometry::build(vm, geometrySetup);
  auto tGeometry = geometry.first;

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

  /// The material mapping algorithm
  FW::MaterialMapping::Config mmAlgConfig(geoContext, mfContext);
  mmAlgConfig.materialMapper   = smm;
  mmAlgConfig.trackingGeometry = tGeometry;

  // Get the file name from the options
  std::string materialFileName = vm["mat-output-file"].as<std::string>();

  if (vm["output-root"].template as<bool>()) {

    /// The writer of the indexed material
    FW::Root::RootMaterialWriter::Config rimConfig("MaterialWriter");
    rimConfig.fileName = materialFileName + ".root";
    auto rimRootWriter
        = std::make_shared<FW::Root::RootMaterialWriter>(rimConfig);

    mmAlgConfig.materialWriters.push_back(rimRootWriter);
  }

  if (vm["output-json"].template as<bool>()) {

    /// The name of the output file
    std::string fileName = vm["mat-output-file"].template as<std::string>();
    // the material writer
    FW::Json::JsonGeometryConverter::Config jmConverterCfg(
        "JsonGeometryConverter", Acts::Logging::INFO);
    jmConverterCfg.processSensitives
        = vm["mat-output-sensitives"].template as<bool>();
    jmConverterCfg.processApproaches
        = vm["mat-output-approaches"].template as<bool>();
    jmConverterCfg.processRepresenting
        = vm["mat-output-representing"].template as<bool>();
    jmConverterCfg.processBoundaries
        = vm["mat-output-boundaries"].template as<bool>();
    jmConverterCfg.processVolumes = vm["mat-output-volume"].template as<bool>();
    jmConverterCfg.writeData      = vm["mat-output-data"].template as<bool>();
    // The writer
    auto jimJsonWriter = std::make_shared<FW::Json::JsonMaterialWriter>(
        jmConverterCfg, materialFileName + ".json";);

    mmAlgConfig.materialWriters.push_back(jimJsonWriter);
  }

  // Create the material mapping
  auto mmAlg = std::make_shared<FW::MaterialMapping>(mmAlgConfig);

  // Apend the Algorithm
  sequencer.appendAlgorithm(mmAlg);

  // Initiate the run
  sequencer.run();
  // Return success code
  return 0;
}
