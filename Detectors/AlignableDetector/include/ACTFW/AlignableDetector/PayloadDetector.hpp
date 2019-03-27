// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include "ACTFW/AlignableDetector/PayloadDecorator.hpp"
#include "ACTFW/AlignableDetector/PayloadDetectorElement.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/GenericDetector/GenericDetectorOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldScalor.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"

using PayloadDetectorElement = FW::Alignable::PayloadDetectorElement;

/// @brief adding some specific options for this geometry type
struct PayloadOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  // @tparam options_t Type of the options object
  ///@param opt Options object
  template <typename options_t>
  void
  operator()(options_t& opt)
  {
    /// Add the generic geometry options
    FW::Options::addGenericGeometryOptions(opt);
    // specify the rotation setp
    opt.add_options()(
        "align-rotation-step",
        boost::program_options::value<double>()->default_value(0.25 * M_PI),
        "Rotation step of the RotationDecorator");
  }
};

/// @brief Algorithm context decorators
struct PayloadContext
{

  /// @brief operator called to construct the context decorators
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  ///
  /// @param vm the parameter map object
  /// @param tGeometry The tracking Geometry
  template <typename variable_map_t>
  std::vector<std::shared_ptr<FW::IContextDecorator>>
  operator()(variable_map_t&                               vm,
             std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
  {

    std::vector<std::shared_ptr<FW::IContextDecorator>> cDecorators;

    // Alignment service
    FW::Alignable::PayloadDecorator::Config agcsConfig;
    agcsConfig.trackingGeometry = tGeometry;
    agcsConfig.rotationStep = vm["align-rotation-step"].template as<double>();
    // Create the service
    auto agcDecorator
        = std::make_shared<FW::Alignable::PayloadDecorator>(agcsConfig);
    cDecorators.push_back(agcDecorator);

    if (vm["bf-context-scalable"].template as<bool>()) {
      FW::BField::BFieldScalor::Config bfsConfig;
      bfsConfig.scalor = vm["bf-bscalor"].template as<double>();

      auto bfDecorator = std::make_shared<FW::BField::BFieldScalor>(bfsConfig);

      cDecorators.push_back(bfDecorator);
    }
    // return the decorators
    return cDecorators;
  }
};

/// @brief geometry getter, the operator() will be called int he example base
struct PayloadGeometry
{

  /// The Store of the detector elements
  std::vector<std::vector<std::shared_ptr<PayloadDetectorElement>>>
      detectorStore;

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
    PayloadDetectorElement::GeometryContext geoContext;
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::Level(
        vm["geo-surface-loglevel"].template as<size_t>());
    Acts::Logging::Level layerLogLevel
        = Acts::Logging::Level(vm["geo-layer-loglevel"].template as<size_t>());
    Acts::Logging::Level volumeLogLevel
        = Acts::Logging::Level(vm["geo-volume-loglevel"].template as<size_t>());
    /// return the generic detector
    return FW::Generic::buildDetector<PayloadDetectorElement>(geoContext,
                                                              detectorStore,
                                                              0,
                                                              surfaceLogLevel,
                                                              layerLogLevel,
                                                              volumeLogLevel);
  }
};
