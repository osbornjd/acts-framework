// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include "ACTFW/AlignableDetector/BuildAlignableDetector.hpp"
#include "ACTFW/AlignableDetector/GeometryRotationDecorator.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/Plugins/BField/BFieldScalor.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"

/// @brief adding some specific options for this geometry type
struct AlignableOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  // @tparam options_t Type of the options object
  ///@param opt Options object
  template <typename options_t>
  void
  operator()(options_t& opt)
  {

    opt.add_options()(
        "align-rotation-step",
        boost::program_options::value<double>()->default_value(0.25 * M_PI),
        "Rotation step of the RotationDecorator");
  }
};

/// @brief geometry getter, the operator() will be called int he example base
struct AlignableGeometry
{
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
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::Level(
        vm["geo-surface-loglevel"].template as<size_t>());
    Acts::Logging::Level layerLogLevel
        = Acts::Logging::Level(vm["geo-layer-loglevel"].template as<size_t>());
    Acts::Logging::Level volumeLogLevel
        = Acts::Logging::Level(vm["geo-volume-loglevel"].template as<size_t>());
    /// return the generic detector
    return FW::Alignable::buildAlignableDetector(
        surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);
  }
};

/// @brief Algorithm context decorators
struct AlignableContext
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
    FW::Alignable::GeometryRotationDecorator::Config agcsConfig;
    agcsConfig.trackingGeometry = tGeometry;
    agcsConfig.rotationStep = vm["align-rotation-step"].template as<double>();
    // Create the service
    auto agcDecorator
        = std::make_shared<FW::Alignable::GeometryRotationDecorator>(
            agcsConfig);
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