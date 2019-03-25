// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"

using GenericDetectorElement = FW::Generic::GenericDetectorElement;

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
  }
};

/// @brief Alignable context service getter
struct GenericContext
{

  /// @brief operator called to construct the context decorators
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  ///
  /// @param vm the parameter map object
  /// @param tGeometry The tracking Geometry
  template <typename variable_map_t>
  std::vector<std::shared_ptr<FW::IContextDecorator>>
  operator()(variable_map_t& /*vm*/,
             std::shared_ptr<const Acts::TrackingGeometry> /*tGeometry*/)
  {
    return {};
  }
};

/// @brief geometry getter, the operator() will be called int he example base
struct GenericGeometry
{

  /// The Store of the detector elements
  std::vector<std::vector<GenericDetectorElement>> detectorStore;

  /// @brief operator called to construct the tracking geometry
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  ///
  /// @param vm the parameter map object
  ///
  /// @return a closed TrackingGeometry object
  template <typename variable_map_t>
  std::shared_ptr<const Acts::TrackingGeometry>
  operator()(variable_map_t& vm)
  {
    // --------------------------------------------------------------------------------
    GenericDetectorElement::GeometryContext geoContext;
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::Level(
        vm["geo-surface-loglevel"].template as<size_t>());
    Acts::Logging::Level layerLogLevel
        = Acts::Logging::Level(vm["geo-layer-loglevel"].template as<size_t>());
    Acts::Logging::Level volumeLogLevel
        = Acts::Logging::Level(vm["geo-volume-loglevel"].template as<size_t>());
    /// return the generic detector
    return FW::Generic::buildDetector<GenericDetectorElement>(geoContext,
                                                              surfaceLogLevel,
                                                              layerLogLevel,
                                                              volumeLogLevel,
                                                              detectorStore,
                                                              3);
  }
};
