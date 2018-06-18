// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/RootDetector/BuildRootDetector.hpp"
#include "ACTFW/RootDetector/RootDetectorOptions.hpp"
#include "TGeoManager.h"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Material/Material.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Plugins/TGeoPlugins/TGeoDetectorElement.hpp"
#include "Acts/Tools/CylinderVolumeBuilder.hpp"
#include "Acts/Tools/CylinderVolumeHelper.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Tools/PassiveLayerBuilder.hpp"
#include "Acts/Tools/SurfaceArrayCreator.hpp"
#include "Acts/Tools/TrackingGeometryBuilder.hpp"
#include "Acts/Tools/TrackingVolumeArrayCreator.hpp"
#include <vector>

namespace FW {
namespace Root {
  
/// @brief global method to build the generic tracking geometry 
// from a TGeo object.
///
/// It does *currently* not translate the material, this has
/// to be done with a material mapping stage
///
/// @tparam variable_map_t is the variable map  
///
/// @param vm is the variable map from the options
  template <typename variable_maps_t>
  std::shared_ptr<const Acts::TrackingGeometry>
  buildRootDetector(variable_maps_t& vm)
  {
  
    Acts::Logging::Level surfaceLogLevel
        = Acts::Logging::Level(vm["sloglevel"].template as<size_t>());
    Acts::Logging::Level layerLogLevel
        = Acts::Logging::Level(vm["lloglevel"].template as<size_t>());
    Acts::Logging::Level volumeLogLevel
        = Acts::Logging::Level(vm["vloglevel"].template as<size_t>());
      
    // configure surface array creator
    auto surfaceArrayCreator = std::make_shared<const Acts::SurfaceArrayCreator>(
        Acts::getDefaultLogger("SurfaceArrayCreator", surfaceLogLevel));
    // configure the layer creator that uses the surface array creator
    Acts::LayerCreator::Config lcConfig;
    lcConfig.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator            = std::make_shared<const Acts::LayerCreator>(
        lcConfig, Acts::getDefaultLogger("LayerCreator", layerLogLevel));
    // configure the layer array creator
    auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
        Acts::getDefaultLogger("LayerArrayCreator", layerLogLevel));
    // tracking volume array creator
    auto tVolumeArrayCreator
        = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
            Acts::getDefaultLogger("TrackingVolumeArrayCreator", volumeLogLevel));
    // configure the cylinder volume helper
    Acts::CylinderVolumeHelper::Config cvhConfig;
    cvhConfig.layerArrayCreator          = layerArrayCreator;
    cvhConfig.trackingVolumeArrayCreator = tVolumeArrayCreator;
    auto cylinderVolumeHelper
        = std::make_shared<const Acts::CylinderVolumeHelper>(
            cvhConfig,
            Acts::getDefaultLogger("CylinderVolumeHelper", volumeLogLevel));
    //-------------------------------------------------------------------------------------
          
    // list the volume builders
    std::list<std::shared_ptr<const Acts::ITrackingVolumeBuilder>> volumeBuilders;

    std::string rootFileName = vm["geo-root-filename"].template as<std::string>();
    // import the file from 
    TGeoManager::Import(rootFileName.c_str());

    bool firstOne = true;
      
    auto layerBuilderConfigs = FW::Options::readRootLayerBuilderConfigs<variable_maps_t>(vm,layerCreator);
    for (auto& lbc : layerBuilderConfigs){
      auto layerBuilder
         = std::make_shared<const Acts::TGeoLayerBuilder>(lbc);
      // build the pixel volume
      Acts::CylinderVolumeBuilder::Config volumeConfig;
      volumeConfig.trackingVolumeHelper = cylinderVolumeHelper;
      volumeConfig.volumeName           = lbc.configurationName;
      volumeConfig.buildToRadiusZero    = firstOne;
      volumeConfig.layerEnvelopeR       = {1. * Acts::units::_mm, 5. * Acts::units::_mm};
      volumeConfig.layerBuilder         = layerBuilder;
      volumeConfig.volumeSignature      = 0;
      auto volumeBuilder = std::make_shared<const Acts::CylinderVolumeBuilder>(
          volumeConfig,
          Acts::getDefaultLogger("VolumeBuilder", volumeLogLevel));
      // add to the list of builders
      volumeBuilders.push_back(volumeBuilder);
      // remember that you've built to the beam pipe already
      firstOne = false;
    }
    
    //-------------------------------------------------------------------------------------
    // create the tracking geometry
    Acts::TrackingGeometryBuilder::Config tgConfig;
    tgConfig.trackingVolumeBuilders = volumeBuilders;
    tgConfig.trackingVolumeHelper   = cylinderVolumeHelper;
    auto cylinderGeometryBuilder
        = std::make_shared<const Acts::TrackingGeometryBuilder>(
            tgConfig,
            Acts::getDefaultLogger("TrackerGeometryBuilder", volumeLogLevel));
    // get the geometry
    auto trackingGeometry = cylinderGeometryBuilder->trackingGeometry();
    /// return the tracking geometry
    return trackingGeometry;
  
  }
                     
} // namespace Root
} // namespace FW