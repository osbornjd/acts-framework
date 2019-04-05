// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include "ACTFW/DD4hepDetector/DD4hepDetectorOptions.hpp"
#include "ACTFW/DD4hepDetector/DD4hepGeometryService.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/GeometryContext.hpp"

using TrackingGeometryPtr = std::shared_ptr<const Acts::TrackingGeometry>;
using ContextDecorators   = std::vector<std::shared_ptr<FW::IContextDecorator>>;

/// @brief adding some specific options for this geometry type
struct DD4hepOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  // @tparam options_t Type of the options object
  ///@param opt Options object
  template <typename options_t>
  void
  operator()(options_t& opt)
  {
    FW::Options::addDD4hepOptions<options_t>(opt);
  }
};

/// @brief geometry getter, the operator() will be called int the example base
struct DD4hepGeometry
{
  /// @brief operator called to construct the tracking geometry and create
  /// optionally the geometry context decorator(s)
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  /// @param vm the parameter map object
  ///
  /// @return a TrackingGeometry object, and optional context decorator(s)
  template <typename variable_map_t>
  std::pair<TrackingGeometryPtr, ContextDecorators>
  operator()(variable_map_t& vm)
  {
    Acts::GeometryContext dd4HepContext;
    // read the detector config & dd4hep detector
    auto dd4HepDetectorConfig
        = FW::Options::readDD4hepConfig<po::variables_map>(vm);
    auto geometrySvc = std::make_shared<FW::DD4hep::DD4hepGeometryService>(
        dd4HepDetectorConfig);
    TrackingGeometryPtr dd4tGeometry
        = geometrySvc->trackingGeometry(dd4HepContext);
    ContextDecorators dd4ContextDeocrators = {};
    // return the pair of geometry and empty decorators
    return std::make_pair<TrackingGeometryPtr, ContextDecorators>(
        std::move(dd4tGeometry), std::move(dd4ContextDeocrators));
  }
};
