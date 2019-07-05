// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"
#include "ACTFW/GenericDetector/GenericDetectorOptions.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"

using DetectorElement     = FW::Generic::GenericDetectorElement;
using DetectorElementPtr  = std::shared_ptr<DetectorElement>;
using TrackingGeometryPtr = std::shared_ptr<const Acts::TrackingGeometry>;
using ContextDecorators   = std::vector<std::shared_ptr<FW::IContextDecorator>>;
using DetectorStore       = std::vector<std::vector<DetectorElementPtr>>;

/// @brief adding some specific options for this geometry type
struct GenericOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  // @tparam options_t Type of the options object
  ///@param opt Options object
  template <typename options_t>
  void
  operator()(options_t& opt)
  {
    FW::Options::addGenericGeometryOptions(opt);
  }
};

/// @brief geometry getter, the operator() will be called int the example base
struct GenericGeometry
{
  /// The Store of the detector elements (lifetime: job)
  DetectorStore detectorStore;

  /// @brief operator called to construct the tracking geometry and create
  /// optionally the geometry context decorator(s)
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  /// @tparam material_decorator_t Type of the material decorator
  ///
  /// @param vm the parameter map object
  /// @param mdecorator the actual material decorator
  ///
  /// @return a TrackingGeometry object, and optional context decorator(s)
  template <typename variable_map_t, typename material_decorator_t>
  std::pair<TrackingGeometryPtr, ContextDecorators>
  operator()(variable_map_t& vm, material_decorator_t mdecorator)
  {
    // --------------------------------------------------------------------------------
    DetectorElement::ContextType nominalContext;

    auto buildLevel = vm["geo-generic-buildlevel"].template as<size_t>();
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::Level(
        vm["geo-surface-loglevel"].template as<size_t>());
    Acts::Logging::Level layerLogLevel
        = Acts::Logging::Level(vm["geo-layer-loglevel"].template as<size_t>());
    Acts::Logging::Level volumeLogLevel
        = Acts::Logging::Level(vm["geo-volume-loglevel"].template as<size_t>());

    bool buildProto
        = (vm["mat-input-type"].template as<std::string>() == "proto");

    /// Return the generic detector
    TrackingGeometryPtr gGeometry
        = FW::Generic::buildDetector<DetectorElement>(nominalContext,
                                                      detectorStore,
                                                      buildLevel,
                                                      std::move(mdecorator),
                                                      buildProto,
                                                      surfaceLogLevel,
                                                      layerLogLevel,
                                                      volumeLogLevel);
    ContextDecorators gContextDeocrators = {};
    // return the pair of geometry and empty decorators
    return std::make_pair<TrackingGeometryPtr, ContextDecorators>(
        std::move(gGeometry), std::move(gContextDeocrators));
  }
};
