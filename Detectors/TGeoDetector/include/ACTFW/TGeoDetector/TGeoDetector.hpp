// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/TGeoDetector/BuildTGeoDetector.hpp"
#include "Acts/Geometry/GeometryContext.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Plugins/TGeo/TGeoDetectorElement.hpp"

using DetectorElementPtr  = std::shared_ptr<const Acts::TGeoDetectorElement>;
using TrackingGeometryPtr = std::shared_ptr<const Acts::TrackingGeometry>;
using ContextDecorators   = std::vector<std::shared_ptr<FW::IContextDecorator>>;
using DetectorStore       = std::vector<DetectorElementPtr>;

/// @brief adding some specific options for this geometry type
struct TGeoOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  // @tparam options_t Type of the options object
  /// @param opt Options object to which dedicated job options can be attached
  template <typename options_t>
  void
  operator()(options_t& opt)
  {
    FW::Options::addTGeoGeometryOptions<options_t>(opt);
  }
};

/// @brief geometry getter, the operator() will be called int the example base
struct TGeoGeometry
{

  DetectorStore detectorStore;

  //// @brief operator called to construct the tracking geometry and create
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
  operator()(variable_map_t& vm, material_decorator_t /*mdecorator*/)
  {
    Acts::GeometryContext tGeoContext;
    TrackingGeometryPtr   tgeoTrackingGeometry
        = FW::TGeo::buildTGeoDetector<variable_map_t>(
            vm, tGeoContext, detectorStore);

    ContextDecorators tgeoContextDeocrators = {};
    // return the pair of geometry and empty decorators
    return std::make_pair<TrackingGeometryPtr, ContextDecorators>(
        std::move(tgeoTrackingGeometry), std::move(tgeoContextDeocrators));
  }
};
