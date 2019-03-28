// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include "ACTFW/ContextualDetector/PayloadDecorator.hpp"
#include "ACTFW/ContextualDetector/PayloadDetectorElement.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/GenericDetector/GenericDetectorOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldScalor.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"

using DetectorElement     = FW::Contextual::PayloadDetectorElement;
using DetectorElementPtr  = std::shared_ptr<DetectorElement>;
using TrackingGeometryPtr = std::shared_ptr<const Acts::TrackingGeometry>;
using Decorator           = FW::Contextual::PayloadDecorator;
using ContextDecorators   = std::vector<std::shared_ptr<FW::IContextDecorator>>;
using DetectorStore       = std::vector<std::vector<DetectorElementPtr>>;

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
    // Add the bfield options for the magnetic field scaling
    FW::Options::addBFieldOptions(opt);
    // specify the rotation setp
    opt.add_options()(
        "align-rotation-step",
        boost::program_options::value<double>()->default_value(0.25 * M_PI),
        "Rotation step of the RotationDecorator")(
        "align-decorator-loglevel",
        boost::program_options::value<size_t>()->default_value(3),
        "Output log level of the alignment decorator.");
  }
};

/// @brief geometry getter, the operator() will be called int he example base
struct PayloadGeometry
{
  /// The Store of the detector elements (lifetime: job)
  DetectorStore detectorStore;

  /// @brief operator called to construct the tracking geometry and create
  /// optionally the geometry context decorator(s)
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  /// @param vm the parameter map object
  ///
  /// @return a TrackingGeometry object, and optional context decoratos(s)
  template <typename variable_map_t>
  std::pair<TrackingGeometryPtr, ContextDecorators>
  operator()(variable_map_t& vm)
  {
    // --------------------------------------------------------------------------------
    DetectorElement::ContextType nominalContext;
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::Level(
        vm["geo-surface-loglevel"].template as<size_t>());
    Acts::Logging::Level layerLogLevel
        = Acts::Logging::Level(vm["geo-layer-loglevel"].template as<size_t>());
    Acts::Logging::Level volumeLogLevel
        = Acts::Logging::Level(vm["geo-volume-loglevel"].template as<size_t>());

    /// return the generic detector
    TrackingGeometryPtr pTrackingGeometry
        = FW::Generic::buildDetector<DetectorElement>(nominalContext,
                                                      detectorStore,
                                                      0,
                                                      surfaceLogLevel,
                                                      layerLogLevel,
                                                      volumeLogLevel);

    ContextDecorators pContextDecorators = {};

    // Alignment service
    Decorator::Config agcsConfig;
    agcsConfig.trackingGeometry = pTrackingGeometry;
    agcsConfig.rotationStep = vm["align-rotation-step"].template as<double>();

    Acts::Logging::Level decoratorLogLevel = Acts::Logging::Level(
        vm["align-decorator-loglevel"].template as<size_t>());

    // Create the service
    auto agcDecorator = std::make_shared<Decorator>(
        agcsConfig,
        Acts::getDefaultLogger("PayloadDecorator", decoratorLogLevel));
    pContextDecorators.push_back(agcDecorator);

    if (vm["bf-context-scalable"].template as<bool>()) {
      FW::BField::BFieldScalor::Config bfsConfig;
      bfsConfig.scalor = vm["bf-bscalor"].template as<double>();

      auto bfDecorator = std::make_shared<FW::BField::BFieldScalor>(bfsConfig);

      pContextDecorators.push_back(bfDecorator);
    }

    // return the pair of geometry and the alignment decorator(s)
    return std::make_pair<TrackingGeometryPtr, ContextDecorators>(
        std::move(pTrackingGeometry), std::move(pContextDecorators));
  }
};
