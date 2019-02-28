// This file is part of the Acts project.
//
// Copyright (C) 2019Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/AlignableDetector/BuildAlignableDetector.hpp"
#include <array>
#include <cmath>
#include <iostream>
#include <vector>
#include "ACTFW/AlignableDetector/AlignableLayerBuilder.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Material/Material.hpp"
#include "Acts/Tools/CylinderVolumeBuilder.hpp"
#include "Acts/Tools/CylinderVolumeHelper.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Tools/LayerCreator.hpp"
#include "Acts/Tools/PassiveLayerBuilder.hpp"
#include "Acts/Tools/SurfaceArrayCreator.hpp"
#include "Acts/Tools/TrackingGeometryBuilder.hpp"
#include "Acts/Tools/TrackingVolumeArrayCreator.hpp"
#include "Acts/Utilities/Units.hpp"

namespace FW {

namespace Alignable {

  std::unique_ptr<const Acts::TrackingGeometry>
  buildAlignableDetector(Acts::Logging::Level surfaceLLevel,
                         Acts::Logging::Level layerLLevel,
                         Acts::Logging::Level volumeLLevel,
                         size_t               stage)
  {

    auto buildContext = std::make_any<AlignableContext>();

    // configure surface array creator
    Acts::SurfaceArrayCreator::Config sacConfig;
    sacConfig.buildContext = buildContext;
    auto surfaceArrayCreator
        = std::make_shared<const Acts::SurfaceArrayCreator>(
            sacConfig,
            Acts::getDefaultLogger("SurfaceArrayCreator", surfaceLLevel));
    // configure the layer creator that uses the surface array creator
    Acts::LayerCreator::Config lcConfig;
    lcConfig.buildContext        = buildContext;
    lcConfig.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator            = std::make_shared<const Acts::LayerCreator>(
        lcConfig, Acts::getDefaultLogger("LayerCreator", layerLLevel));
    // configure the layer array creator
    Acts::LayerArrayCreator::Config lacConfig;
    lacConfig.buildContext = buildContext;
    auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
        lacConfig, Acts::getDefaultLogger("LayerArrayCreator", layerLLevel));
    // tracking volume array creator
    Acts::TrackingVolumeArrayCreator::Config tvacConfig;
    tvacConfig.buildContext = buildContext;
    auto tVolumeArrayCreator
        = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
            tvacConfig,
            Acts::getDefaultLogger("TrackingVolumeArrayCreator", volumeLLevel));
    // configure the cylinder volume helper
    Acts::CylinderVolumeHelper::Config cvhConfig;
    cvhConfig.buildContext               = buildContext;
    cvhConfig.layerArrayCreator          = layerArrayCreator;
    cvhConfig.trackingVolumeArrayCreator = tVolumeArrayCreator;
    auto cylinderVolumeHelper
        = std::make_shared<const Acts::CylinderVolumeHelper>(
            cvhConfig,
            Acts::getDefaultLogger("CylinderVolumeHelper", volumeLLevel));
    //-------------------------------------------------------------------------------------
    // list the volume builders
    std::list<std::shared_ptr<const Acts::ITrackingVolumeBuilder>>
        volumeBuilders;

    //-------------------------------------------------------------------------------------
    // Beam Pipe
    //-------------------------------------------------------------------------------------
    // configure the beam pipe layer builder
    Acts::PassiveLayerBuilder::Config bplConfig;
    bplConfig.buildContext            = buildContext;
    bplConfig.layerIdentification     = "BeamPipe";
    bplConfig.centralLayerRadii       = std::vector<double>(1, 19.);
    bplConfig.centralLayerHalflengthZ = std::vector<double>(1, 3000.);
    bplConfig.centralLayerThickness   = std::vector<double>(1, 0.8);
    bplConfig.centralLayerMaterial
        = {std::make_shared<Acts::HomogeneousSurfaceMaterial>(
            Acts::MaterialProperties(352.8, 407., 9.012, 4., 1.848e-3, 0.8))};
    auto beamPipeBuilder = std::make_shared<const Acts::PassiveLayerBuilder>(
        bplConfig, Acts::getDefaultLogger("BeamPipeLayerBuilder", layerLLevel));
    // create the volume for the beam pipe
    Acts::CylinderVolumeBuilder::Config bpvConfig;
    bpvConfig.buildContext         = buildContext;
    bpvConfig.trackingVolumeHelper = cylinderVolumeHelper;
    bpvConfig.volumeName           = "BeamPipe";
    bpvConfig.layerBuilder         = beamPipeBuilder;
    bpvConfig.layerEnvelopeR = {1. * Acts::units::_mm, 1. * Acts::units::_mm};
    bpvConfig.buildToRadiusZero = true;
    bpvConfig.volumeSignature   = 0;
    auto beamPipeVolumeBuilder
        = std::make_shared<const Acts::CylinderVolumeBuilder>(
            bpvConfig,
            Acts::getDefaultLogger("BeamPipeVolumeBuilder", volumeLLevel));
    // add to the list of builders
    volumeBuilders.push_back(beamPipeVolumeBuilder);

    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // Pixel detector
    //-------------------------------------------------------------------------------------
    // configure pixel layer builder
    FW::Alignable::AlignableLayerBuilder::Config plbConfig;
    plbConfig.layerCreator        = layerCreator;
    plbConfig.layerIdentification = "Pixel";
    plbConfig.buildContext        = buildContext;
    // fill necessary vectors for configuration
    //-------------------------------------------------------------------------------------
    // some prep work
    // envelope for layers
    std::pair<double, double> pcEnvelope(2., 2.);
    // Layer material properties - thickness, X0, L0, A, Z, Rho
    Acts::MaterialProperties pcmbProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 1.5 * Acts::units::_mm);
    Acts::MaterialProperties pcmecProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 1.5 * Acts::units::_mm);
    // Module material - X0, L0, A, Z, Rho
    Acts::Material pcMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

    // standard, an approach envelope
    plbConfig.approachSurfaceEnvelope = 9.;

    // BARREL :
    // 4 pixel layers
    // configure the central barrel
    plbConfig.centralLayerBinMultipliers = {1, 1};
    plbConfig.centralLayerRadii          = {32., 72., 116., 172.};
    plbConfig.centralLayerEnvelopes
        = {pcEnvelope, pcEnvelope, pcEnvelope, pcEnvelope};
    // material concentration alsways outside the modules
    plbConfig.centralLayerMaterialConcentration = {1, 1, 1, 1};
    plbConfig.centralLayerMaterialProperties
        = {pcmbProperties, pcmbProperties, pcmbProperties, pcmbProperties};
    plbConfig.centralModuleBinningSchema
        = {{16, 14}, {32, 14}, {52, 14}, {78, 14}};
    plbConfig.centralModuleTiltPhi   = {0.14, 0.14, 0.14, 0.14};
    plbConfig.centralModuleHalfX     = {8.4, 8.4, 8.4, 8.4};
    plbConfig.centralModuleHalfY     = {36., 36., 36., 36.};
    plbConfig.centralModuleThickness = {0.15, 0.15, 0.15, 0.15};
    plbConfig.centralModuleMaterial
        = {pcMaterial, pcMaterial, pcMaterial, pcMaterial};
    // pitch definitions
    plbConfig.centralModuleReadoutBinsX = {336, 336, 336, 336};
    plbConfig.centralModuleReadoutBinsY = {1280, 1280, 1280, 1280};
    plbConfig.centralModuleReadoutSide  = {-1, -1, -1, -1};
    plbConfig.centralModuleLorentzAngle = {0.12, 0.12, 0.12, 0.12};

    // no frontside/backside
    plbConfig.centralModuleFrontsideStereo = {};
    plbConfig.centralModuleBacksideStereo  = {};
    plbConfig.centralModuleBacksideGap     = {};
    // mPositions
    std::vector<std::vector<Acts::Vector3D>> plbCentralModulePositions;
    for (size_t plb = 0; plb < plbConfig.centralLayerRadii.size(); ++plb) {
      // call the helper function
      plbCentralModulePositions.push_back(FW::Generic::modulePositionsCylinder(
          plbConfig.centralLayerRadii[plb],
          0.5,  // 1 mm stagger
          plbConfig.centralModuleHalfY[plb],
          2.,  // 4 mm module overlap in z
          plbConfig.centralModuleBinningSchema[plb]));
    }
    plbConfig.centralModulePositions = plbCentralModulePositions;

    // define the builder
    auto pixelLayerBuilder
        = std::make_shared<const FW::Alignable::AlignableLayerBuilder>(
            plbConfig,
            Acts::getDefaultLogger("PixelLayerBuilder", layerLLevel));
    //-------------------------------------------------------------------------------------
    // build the pixel volume
    Acts::CylinderVolumeBuilder::Config pvbConfig;
    pvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
    pvbConfig.volumeName           = "Pixel";
    pvbConfig.buildToRadiusZero    = false;
    pvbConfig.layerEnvelopeR  = {1. * Acts::units::_mm, 5. * Acts::units::_mm};
    pvbConfig.layerBuilder    = pixelLayerBuilder;
    pvbConfig.volumeSignature = 0;
    auto pixelVolumeBuilder
        = std::make_shared<const Acts::CylinderVolumeBuilder>(
            pvbConfig,
            Acts::getDefaultLogger("PixelVolumeBuilder", volumeLLevel));
    // add to the list of builders
    volumeBuilders.push_back(pixelVolumeBuilder);

    //-------------------------------------------------------------------------------------
    // create the tracking geometry
    Acts::TrackingGeometryBuilder::Config tgConfig;
    tgConfig.trackingVolumeBuilders = volumeBuilders;
    tgConfig.trackingVolumeHelper   = cylinderVolumeHelper;
    auto cylinderGeometryBuilder
        = std::make_shared<const Acts::TrackingGeometryBuilder>(
            tgConfig,
            Acts::getDefaultLogger("TrackerGeometryBuilder", volumeLLevel));
    // get the geometry
    auto trackingGeometry = cylinderGeometryBuilder->trackingGeometry();

    /// return the tracking geometry
    return trackingGeometry;
  }

}  // namespace Alignable
}  // namesapce FW
