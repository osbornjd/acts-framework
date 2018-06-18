// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "FatrasExampleBase.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"


/// @brief geometry getter, the operator() will be called int he example base
struct GenericGeometryGetter {

template <typename options_t>
std::shared_ptr<const Acts::TrackingGeometry> 
operator()(options_t& vm){
  // --------------------------------------------------------------------------------
  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel
      = Acts::Logging::Level(vm["geo-surfaceloglevel"].template as<size_t>());
  Acts::Logging::Level layerLogLevel
      = Acts::Logging::Level(vm["geo-layerloglevel"].template as<size_t>());
  Acts::Logging::Level volumeLogLevel
      = Acts::Logging::Level(vm["geo-volumeloglevel"].template as<size_t>());
  /// return the generic detector 
  return FWGen::buildGenericDetector(
        surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);
} 
};

int
main(int argc, char* argv[])
{
  // --------------------------------------------------------------------------------
  GenericGeometryGetter tGeoGetter;
  // now process it
  return fatrasExample(argc, argv, tGeoGetter);
}