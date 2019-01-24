// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Geant4MaterialRecording.hpp"
#include "MaterialMapping.hpp"

namespace po = boost::program_options;
namespace au = Acts::units;

namespace FW {

namespace Options {

  /// @brief ExtrapolationAlgorithm options
  ///
  /// @tparam aopt_t Type of the options class from boost
  template <typename aopt_t>
  void
  addMaterialMappingOptions(aopt_t& opt)
  {
    opt.add_options()("mm-g4-num-tracks",
                      po::value<unsigned int>()->default_value(1000),
                      "Number of g4 tracks per event.")(
        "mm-g4-material-collection",
        po::value<std::string>()->default_value("g4material"),
        "Geant4 recorded material collection")(
        "mm-g4-gdml-file",
        po::value<std::string>()->default_value(""),
        "GDML file of the detector to be recorded (optional)")(
        "mm-g4-seed1",
        po::value<unsigned int>()->default_value(12345),
        "First seed for Geant4.")(
        "mm-g4-seed2",
        po::value<unsigned int>()->default_value(567890),
        "Second seed for Geant4.");
  }

  /// Read the material mapping options and return a Config file
  ///
  /// @tparam vmap_t is the Type of the Parameter map to be read out
  /// @param vm is the parameter map for the options
  ///
  /// @returns a Config object for the MaterialMapping algorithm
  template <typename vmap_t>
  FW::MaterialMapping::Config
  readMaterialMappingConfig(const vmap_t& vm)
  {
    FW::MaterialMapping::Config mmConfig;

    return mmConfig;
  }

  /// Read the geant4 material recording options and return a Config file
  ///
  /// @tparam vmap_t is the Type of the Parameter map to be read out
  /// @param vm is the parameter map for the options
  ///
  /// @returns a Config object for the MaterialMapping algorithm
  template <typename vmap_t>
  FW::Geant4MaterialRecording::Config
  readG4MaterialRecordingConfig(const vmap_t& vm)
  {
    FW::Geant4MaterialRecording::Config g4rConfig;

    g4rConfig.gdmlFile = vm["mm-g4-gdml-file"].template as<std::string>();
    g4rConfig.recordedMaterialCollection
        = vm["mm-g4-material-collection"].template as<std::string>();
    g4rConfig.tracksPerEvent
        = vm["mm-g4-num-tracks"].template as<unsigned int>();
    g4rConfig.seed1 = vm["mm-g4-seed1"].template as<unsigned int>();
    g4rConfig.seed2 = vm["mm-g4-seed2"].template as<unsigned int>();

    return g4rConfig;
  }

}  // namespace Options
}  // namespace FW
