// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>

#include "ACTFW/ContextualDetector/AlignedDetectorElement.hpp"
#include "ACTFW/ContextualDetector/AlignmentDecorator.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/GenericDetector/GenericDetectorOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldScalor.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"

using DetectorElement     = FW::Contextual::AlignedDetectorElement;
using DetectorElementPtr  = std::shared_ptr<DetectorElement>;
using TrackingGeometryPtr = std::shared_ptr<const Acts::TrackingGeometry>;
using Decorator           = FW::Contextual::AlignmentDecorator;
using ContextDecorators   = std::vector<std::shared_ptr<FW::IContextDecorator>>;
using DetectorStore       = std::vector<std::vector<DetectorElementPtr>>;

/// @brief adding some specific options for this geometry type
struct AlignedOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  /// @tparam options_t Type of the options object
  /// @param opt Options object
  template <typename options_t>
  void
  operator()(options_t& opt)
  {
    // Add the generic geometry options
    FW::Options::addGenericGeometryOptions(opt);
    // Add the bfield options for the magnetic field scaling
    FW::Options::addBFieldOptions(opt);
    // specify the rotation setp
    opt.add_options()(
        "align-seed",
        boost::program_options::value<size_t>()->default_value(1324354657),
        "Seed for the decorator random numbers.")(
        "align-iovsize",
        boost::program_options::value<size_t>()->default_value(100),
        "Size of a valid IOV.")(
        "align-flushsize",
        boost::program_options::value<size_t>()->default_value(200),
        "Span until garbage collection is active.")(
        "align-sigma-iplane",
        boost::program_options::value<double>()->default_value(100.),
        "Sigma of the in-plane misalignment in [um]")(
        "align-sigma-oplane",
        boost::program_options::value<double>()->default_value(50.),
        "Sigma of the out-of-plane misalignment in [um]")(
        "align-sigma-irot",
        boost::program_options::value<double>()->default_value(20.),
        "Sigma of the in-plane rotation misalignment in [mrad]")(
        "align-sigma-orot",
        boost::program_options::value<double>()->default_value(0.),
        "Sigma of the out-of-plane rotation misalignment in [mrad]")(
        "align-loglevel",
        boost::program_options::value<size_t>()->default_value(3),
        "Output log level of the alignment decorator.")(
        "align-firstnominal",
        boost::program_options::value<bool>()->default_value(false),
        "Keep the first iov batch nominal.");
  }
};

/// @brief geometry getter, the operator() will be called int the example base
struct AlignedGeometry
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

    /// return the generic detector - with aligned context decorator
    TrackingGeometryPtr aTrackingGeometry
        = FW::Generic::buildDetector<DetectorElement>(nominalContext,
                                                      detectorStore,
                                                      buildLevel,
                                                      std::move(mdecorator),
                                                      buildProto,
                                                      surfaceLogLevel,
                                                      layerLogLevel,
                                                      volumeLogLevel);

    Acts::Logging::Level decoratorLogLevel
        = Acts::Logging::Level(vm["align-loglevel"].template as<size_t>());

    // Let's create a reandom number service
    FW::RandomNumbersSvc::Config randomNumberConfig;
    randomNumberConfig.seed = vm["align-seed"].template as<size_t>();
    auto randomNumberSvc
        = std::make_shared<FW::RandomNumbersSvc>(randomNumberConfig);

    // Alignment decorator service
    Decorator::Config agcsConfig;
    agcsConfig.detectorStore = detectorStore;
    agcsConfig.iovSize       = vm["align-iovsize"].template as<size_t>();
    agcsConfig.flushSize     = vm["align-flushsize"].template as<size_t>();

    // The misalingments
    double sigmaIp             = vm["align-sigma-iplane"].template as<double>();
    double sigmaOp             = vm["align-sigma-oplane"].template as<double>();
    double sigmaIr             = vm["align-sigma-irot"].template as<double>();
    double sigmaOr             = vm["align-sigma-orot"].template as<double>();
    agcsConfig.gSigmaX         = sigmaIp * Acts::units::_um;
    agcsConfig.gSigmaY         = sigmaIp * Acts::units::_um;
    agcsConfig.gSigmaZ         = sigmaOp * Acts::units::_um;
    agcsConfig.aSigmaX         = sigmaOr * 0.001;  // millirad
    agcsConfig.aSigmaY         = sigmaOr * 0.001;  // millirad
    agcsConfig.aSigmaZ         = sigmaIr * 0.001;  // millirad
    agcsConfig.randomNumberSvc = randomNumberSvc;
    agcsConfig.firstIovNominal = vm["align-firstnominal"].template as<bool>();

    // Now create the alignment decorator
    ContextDecorators aContextDecorators = {std::make_shared<Decorator>(
        agcsConfig,
        Acts::getDefaultLogger("AlignmentDecorator", decoratorLogLevel))};

    if (vm["bf-context-scalable"].template as<bool>()) {
      FW::BField::BFieldScalor::Config bfsConfig;
      bfsConfig.scalor = vm["bf-bscalor"].template as<double>();

      auto bfDecorator = std::make_shared<FW::BField::BFieldScalor>(bfsConfig);

      aContextDecorators.push_back(bfDecorator);
    }

    // return the pair of geometry and the alignment decorator(s)
    return std::make_pair<TrackingGeometryPtr, ContextDecorators>(
        std::move(aTrackingGeometry), std::move(aContextDecorators));
  }
};
