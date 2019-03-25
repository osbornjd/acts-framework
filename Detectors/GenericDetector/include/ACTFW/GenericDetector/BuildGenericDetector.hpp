// This file is part of the Acts project.
//
// Copyright (C) 2016-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <array>
#include <cmath>
#include <iostream>
#include <list>
#include <memory>
#include <vector>
#include "ACTFW/GenericDetector/LayerBuilderT.hpp"
#include "ACTFW/GenericDetector/ProtoLayerCreatorT.hpp"
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
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"

namespace Acts {
class TrackingGeometry;
}

namespace FW {
namespace Generic {

  /// Helper method for positioning
  /// @param radius is the cylinder radius
  /// @param zStagger is the radial staggering along z
  /// @param moduleHalfLength is the module length (longitudinal)
  /// @param lOverlap is the overlap of the modules (longitudinal)
  /// @binningSchema is the way the bins are laid out rphi x z
  std::vector<Acts::Vector3D>
  modulePositionsCylinder(double radius,
                          double zStagger,
                          double moduleHalfLength,
                          double lOverlap,
                          const std::pair<int, int>& binningSchema);

  /// Helper method for positioning
  /// @param z is the z position of the ring
  /// @param radius is the ring radius
  /// @param phiStagger is the radial staggering along phi
  /// @param lOverlap is the overlap of the modules
  /// @parm nPhiBins is the number of bins in phi
  std::vector<Acts::Vector3D>
  modulePositionsRing(double z,
                      double radius,
                      double phiStagger,
                      double phiSubStagger,
                      int    nPhiBins);

  /// Helper method for positioning
  /// @param z is the nominal z posiiton of the dis
  /// @param ringStagger is the staggering of the different rings
  /// @param phiStagger is the staggering on a ring in phi : it is even/odd
  /// @param phiSubStagger is the sub staggering on a ring in phi : it affects
  /// 0/4/8 and 3/6
  /// @param innerRadius is the inner Radius for the disc
  /// @param outerRadius is the outer Radius for the disc
  /// @param discBinning is the binning setup in r, phi
  /// @param moduleHalfLength is pair of phibins and module length
  std::vector<std::vector<Acts::Vector3D>>
  modulePositionsDisc(double                     z,
                      double                     ringStagger,
                      std::vector<double>        phiStagger,
                      std::vector<double>        phiSubStagger,
                      double                     innerRadius,
                      double                     outerRadius,
                      const std::vector<size_t>& discBinning,
                      const std::vector<double>& moduleHalfLength);

  /// Global method to build the generic tracking geometry
  /// @param gctx is the detector element dependent geometry context
  /// @param surfaceLLevel is the surface building logging level
  /// @param layerLLevel is the layer building logging level
  /// @param volumeLLevel is the volume building logging level
  /// @param detectorStore is the store for the detector element
  /// @param version is the detector version
  /// return a unique vector to the tracking geometry
  template <typename detector_element_t>
  std::unique_ptr<const Acts::TrackingGeometry>
  buildDetector(const typename detector_element_t::GeometryContext& gctx,
                Acts::Logging::Level surfaceLLevel = Acts::Logging::INFO,
                Acts::Logging::Level layerLLevel   = Acts::Logging::INFO,
                Acts::Logging::Level volumeLLevel  = Acts::Logging::INFO,
                std::vector<std::vector<GenericDetectorElement>> detectorStore
                = {},
                size_t version = 0)
  {

    using ProtoLayerCreator = ProtoLayerCreatorT<detector_element_t>;
    using LayerBuilder      = LayerBuilderT<detector_element_t>;

    // configure surface array creator
    Acts::SurfaceArrayCreator::Config sacConfig;
    auto                              surfaceArrayCreator
        = std::make_shared<const Acts::SurfaceArrayCreator>(
            sacConfig,
            Acts::getDefaultLogger("SurfaceArrayCreator", surfaceLLevel));
    // configure the layer creator that uses the surface array creator
    Acts::LayerCreator::Config lcConfig;
    lcConfig.surfaceArrayCreator = surfaceArrayCreator;
    auto layerCreator            = std::make_shared<const Acts::LayerCreator>(
        lcConfig, Acts::getDefaultLogger("LayerCreator", layerLLevel));
    // configure the layer array creator
    Acts::LayerArrayCreator::Config lacConfig;
    auto layerArrayCreator = std::make_shared<const Acts::LayerArrayCreator>(
        lacConfig, Acts::getDefaultLogger("LayerArrayCreator", layerLLevel));
    // tracking volume array creator
    Acts::TrackingVolumeArrayCreator::Config tvacConfig;
    auto                                     tVolumeArrayCreator
        = std::make_shared<const Acts::TrackingVolumeArrayCreator>(
            tvacConfig,
            Acts::getDefaultLogger("TrackingVolumeArrayCreator", volumeLLevel));
    // configure the cylinder volume helper
    Acts::CylinderVolumeHelper::Config cvhConfig;
    cvhConfig.layerArrayCreator          = layerArrayCreator;
    cvhConfig.trackingVolumeArrayCreator = tVolumeArrayCreator;
    auto cylinderVolumeHelper
        = std::make_shared<const Acts::CylinderVolumeHelper>(
            cvhConfig,
            Acts::getDefaultLogger("CylinderVolumeHelper", volumeLLevel));
    //-------------------------------------------------------------------------------------
    // vector of the volume builders
    std::vector<std::shared_ptr<const Acts::ITrackingVolumeBuilder>>
        volumeBuilders;

    // This file is part of the Acts project.
    //
    // Copyright (C) 2016-2018 Acts project team
    //
    // This Source Code Form is subject to the terms of the Mozilla Public
    // License, v. 2.0. If a copy of the MPL was not distributed with this
    // file, You can obtain one at http://mozilla.org/MPL/2.0/.

    //-------------------------------------------------------------------------------------
    // Beam Pipe
    //-------------------------------------------------------------------------------------
    // configure the beam pipe layer builder
    Acts::PassiveLayerBuilder::Config bplConfig;
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
    // some prep work
    // envelope for layers
    std::pair<double, double> pcEnvelope(2., 2.);
    // Module material - X0, L0, A, Z, Rho
    Acts::Material pcMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);
    // Layer material properties - thickness, X0, L0, A, Z, Rho
    Acts::MaterialProperties pcmbProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 1.5 * Acts::units::_mm);
    Acts::MaterialProperties pcmecProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 1.5 * Acts::units::_mm);
    // configure the pixel proto layer builder
    typename ProtoLayerCreator::Config pplConfig;
    // standard, an approach envelope
    pplConfig.approachSurfaceEnvelope = 1.;
    // BARREL :
    // 4 pixel layers
    // configure the central barrel
    pplConfig.centralLayerBinMultipliers = {1, 1};
    pplConfig.centralLayerRadii          = {32., 72., 116., 172.};
    pplConfig.centralLayerEnvelopes
        = {pcEnvelope, pcEnvelope, pcEnvelope, pcEnvelope};
    pplConfig.centralModuleBinningSchema
        = {{16, 14}, {32, 14}, {52, 14}, {78, 14}};
    pplConfig.centralModuleTiltPhi   = {0.14, 0.14, 0.14, 0.14};
    pplConfig.centralModuleHalfX     = {8.4, 8.4, 8.4, 8.4};
    pplConfig.centralModuleHalfY     = {36., 36., 36., 36.};
    pplConfig.centralModuleThickness = {0.15, 0.15, 0.15, 0.15};
    pplConfig.centralModuleMaterial
        = {pcMaterial, pcMaterial, pcMaterial, pcMaterial};
    // pitch definitions
    pplConfig.centralModuleReadoutBinsX = {336, 336, 336, 336};
    pplConfig.centralModuleReadoutBinsY = {1280, 1280, 1280, 1280};
    pplConfig.centralModuleReadoutSide  = {-1, -1, -1, -1};
    pplConfig.centralModuleLorentzAngle = {0.12, 0.12, 0.12, 0.12};

    // no frontside/backside
    pplConfig.centralModuleFrontsideStereo = {};
    pplConfig.centralModuleBacksideStereo  = {};
    pplConfig.centralModuleBacksideGap     = {};
    // mPositions
    std::vector<std::vector<Acts::Vector3D>> plbCentralModulePositions;
    for (size_t plb = 0; plb < pplConfig.centralLayerRadii.size(); ++plb) {
      // call the helper function
      plbCentralModulePositions.push_back(
          modulePositionsCylinder(pplConfig.centralLayerRadii[plb],
                                  0.5,  // 1 mm stagger
                                  pplConfig.centralModuleHalfY[plb],
                                  2.,  // 4 mm module overlap in z
                                  pplConfig.centralModuleBinningSchema[plb]));
    }
    pplConfig.centralModulePositions = plbCentralModulePositions;
    // ENDCAP :
    // 7 pixel layers each side
    // configure the endcaps
    pplConfig.posnegLayerBinMultipliers = {1, 1};

    pplConfig.posnegLayerPositionsZ = {600. * Acts::units::_mm,
                                       700. * Acts::units::_mm,
                                       820. * Acts::units::_mm,
                                       960. * Acts::units::_mm,
                                       1100 * Acts::units::_mm,
                                       1300 * Acts::units::_mm,
                                       1500 * Acts::units::_mm};

    pplConfig.posnegLayerEnvelopeR = {1. * Acts::units::_mm,
                                      1. * Acts::units::_mm,
                                      1. * Acts::units::_mm,
                                      1. * Acts::units::_mm,
                                      1. * Acts::units::_mm,
                                      1. * Acts::units::_mm,
                                      1. * Acts::units::_mm};
    std::vector<double>         perHX = {8.4, 8.4};    // half length x
    std::vector<double>         perHY = {36., 36.};    // half length y
    std::vector<size_t>         perBP = {40, 68};      // bins in phi
    std::vector<double>         perT  = {0.15, 0.15};  // module thickness
    std::vector<size_t>         perBX = {336, 336};    // bins in x
    std::vector<size_t>         perBY = {1280, 1280};  // bins in y
    std::vector<int>            perRS = {-1, -1};      // readout side
    std::vector<double>         perLA = {0., 0.};      // lorentz angle
    std::vector<Acts::Material> perM  = {pcMaterial, pcMaterial};  // material

    pplConfig.posnegModuleMinHalfX = std::vector<std::vector<double>>(7, perHX);
    pplConfig.posnegModuleMaxHalfX = {};
    pplConfig.posnegModuleHalfY    = std::vector<std::vector<double>>(7, perHY);
    pplConfig.posnegModulePhiBins  = std::vector<std::vector<size_t>>(7, perBP);
    pplConfig.posnegModuleThickness = std::vector<std::vector<double>>(7, perT);
    pplConfig.posnegModuleReadoutBinsX
        = std::vector<std::vector<size_t>>(7, perBX);
    pplConfig.posnegModuleReadoutBinsY
        = std::vector<std::vector<size_t>>(7, perBY);
    pplConfig.posnegModuleReadoutSide = std::vector<std::vector<int>>(7, perRS);
    pplConfig.posnegModuleLorentzAngle
        = std::vector<std::vector<double>>(7, perLA);
    pplConfig.posnegModuleMaterial
        = std::vector<std::vector<Acts::Material>>(7, perM);

    // no frontside/backside
    pplConfig.posnegModuleFrontsideStereo = {};
    pplConfig.posnegModuleBacksideStereo  = {};
    pplConfig.posnegModuleBacksideGap     = {};
    // mPositions
    std::vector<std::vector<std::vector<Acts::Vector3D>>>
        plbPosnegModulePositions;
    for (size_t id = 0; id < pplConfig.posnegLayerPositionsZ.size(); ++id) {
      plbPosnegModulePositions.push_back(
          modulePositionsDisc(pplConfig.posnegLayerPositionsZ[id],
                              0.0,
                              {4.0, 4.0},
                              {0.5, 0.},
                              30.,
                              176.,
                              pplConfig.posnegModulePhiBins[id],
                              pplConfig.posnegModuleHalfY[id]));
    }
    pplConfig.posnegModulePositions = plbPosnegModulePositions;

    /// Create the ProtoLayer creator
    ProtoLayerCreator pplCreator(pplConfig);

    // configure pixel layer builder
    typename LayerBuilder::Config plbConfig;
    plbConfig.layerCreator        = layerCreator;
    plbConfig.layerIdentification = "Pixel";
    // material concentration alsways outside the modules
    plbConfig.centralProtoLayers
        = pplCreator.centralProtoLayers(gctx, detectorStore);
    plbConfig.centralLayerMaterialConcentration = {1, 1, 1, 1};
    plbConfig.centralLayerMaterialProperties
        = {pcmbProperties, pcmbProperties, pcmbProperties, pcmbProperties};
    // material concentration is always behind the layer in the pixels
    plbConfig.posnegLayerMaterialConcentration = std::vector<int>(7, 0);
    // layer structure surface has pixel material properties
    plbConfig.posnegLayerMaterialProperties = {pcmecProperties,
                                               pcmecProperties,
                                               pcmecProperties,
                                               pcmecProperties,
                                               pcmecProperties,
                                               pcmecProperties,
                                               pcmecProperties};
    // negative proto layers
    plbConfig.negativeProtoLayers
        = pplCreator.negativeProtoLayers(gctx, detectorStore);
    plbConfig.centralProtoLayers
        = pplCreator.positiveProtoLayers(gctx, detectorStore);
    // define the builder
    auto pixelLayerBuilder = std::make_shared<const LayerBuilder>(
        plbConfig, Acts::getDefaultLogger("PixelLayerBuilder", layerLLevel));
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
    //-------------------------------------------------------------------------------------
    // Pixel Support Tybe (PST)
    //-------------------------------------------------------------------------------------
    Acts::PassiveLayerBuilder::Config pstConfig;
    pstConfig.layerIdentification     = "PST";
    pstConfig.centralLayerRadii       = std::vector<double>(1, 200.);
    pstConfig.centralLayerHalflengthZ = std::vector<double>(1, 2800.);
    pstConfig.centralLayerThickness   = std::vector<double>(1, 1.8);
    pstConfig.centralLayerMaterial
        = {std::make_shared<Acts::HomogeneousSurfaceMaterial>(
            Acts::MaterialProperties(352.8, 407., 9.012, 4., 1.848e-3, 1.8))};
    auto pstBuilder = std::make_shared<const Acts::PassiveLayerBuilder>(
        pstConfig, Acts::getDefaultLogger("PSTBuilder", layerLLevel));
    // create the volume for the beam pipe
    Acts::CylinderVolumeBuilder::Config pstvolConfig;
    pstvolConfig.trackingVolumeHelper = cylinderVolumeHelper;
    pstvolConfig.volumeName           = "PST";
    pstvolConfig.buildToRadiusZero    = false;
    pstvolConfig.layerBuilder         = pstBuilder;
    pstvolConfig.volumeSignature      = 0;
    auto pstVolumeBuilder = std::make_shared<const Acts::CylinderVolumeBuilder>(
        pstvolConfig, Acts::getDefaultLogger("PSTVolumeBuilder", volumeLLevel));
    // add to the detector builds
    volumeBuilders.push_back(pstVolumeBuilder);
    /*
    //-------------------------------------------------------------------------------------
    // SHORT strip detector
    //-------------------------------------------------------------------------------------
    // first add a Pixel Support Tube
    // STRIPS
    //
    ----------------------------------------------------------------------------
    // configure short strip layer builder
    FW::Generic::LayerBuilder::Config sslbConfig;
    sslbConfig.layerCreator        = layerCreator;
    sslbConfig.layerIdentification = "SStrip";
    // fill necessary vectors for configuration
    //-------------------------------------------------------------------------------------
    // some prep work
    // envelope double
    std::pair<double, double> ssEnvelope(2., 2.);
    // Layer material properties - thickness, X0, L0, A, Z, Rho
    Acts::MaterialProperties ssbmProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 2. * Acts::units::_mm);
    Acts::MaterialProperties ssecmProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 2.5 * Acts::units::_mm);

    // Module material - X0, L0, A, Z, Rho
    Acts::Material ssMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

    // configure the central barrel
    sslbConfig.centralLayerBinMultipliers = {1, 1};
    sslbConfig.centralLayerRadii          = {260., 360., 500., 660.};
    sslbConfig.centralLayerEnvelopes
        = {ssEnvelope, ssEnvelope, ssEnvelope, ssEnvelope};
    sslbConfig.centralLayerMaterialConcentration = {-1, -1, -1, -1};
    sslbConfig.centralLayerMaterialProperties
        = {ssbmProperties, ssbmProperties, ssbmProperties, ssbmProperties};
    sslbConfig.centralModuleBinningSchema
        = {{40, 21}, {56, 21}, {78, 21}, {102, 21}};
    sslbConfig.centralModuleTiltPhi   = {-0.15, -0.15, -0.15, -0.15};
    sslbConfig.centralModuleHalfX     = {24., 24., 24., 24.};
    sslbConfig.centralModuleHalfY     = {54., 54., 54., 54.};
    sslbConfig.centralModuleThickness = {0.25, 0.25, 0.25, 0.25};

    sslbConfig.centralModuleReadoutBinsX = {600, 600, 600, 600};  // 80 um pitch
    sslbConfig.centralModuleReadoutBinsY = {90, 90, 90, 90};  // 1.2 mm strixels
    sslbConfig.centralModuleReadoutSide  = {1, 1, 1, 1};
    sslbConfig.centralModuleLorentzAngle = {0.12, 0.12, 0.12, 0.12};

    sslbConfig.centralModuleMaterial
        = {ssMaterial, ssMaterial, ssMaterial, ssMaterial};
    sslbConfig.centralModuleFrontsideStereo = {};
    sslbConfig.centralModuleBacksideStereo  = {};
    sslbConfig.centralModuleBacksideGap     = {};
    // mPositions
    std::vector<std::vector<Acts::Vector3D>> sslbCentralModulePositions;
    for (size_t sslb = 0; sslb < sslbConfig.centralLayerRadii.size(); ++sslb) {
      // call the helper function
      sslbCentralModulePositions.push_back(
          modulePositionsCylinder(sslbConfig.centralLayerRadii[sslb],
                                  3.,  // 3 mm stagger
                                  sslbConfig.centralModuleHalfY[sslb],
                                  5.,  // 5 mm module overlap
                                  sslbConfig.centralModuleBinningSchema[sslb]));
    }
    sslbConfig.centralModulePositions = sslbCentralModulePositions;

    // configure the endcaps
    std::vector<double> mrMinHx = {16.4, 24.2, 32.2};
    std::vector<double> mrMaxHx = {24.2, 32.2, 40.0};
    std::vector<double> mrHy    = {78., 78., 78.};

    // simplified strixels readout
    std::vector<size_t> mrReadoutBinsX = {605, 805, 1000};  // 80 um pitch
    std::vector<size_t> mrReadoutBinsY = {130, 130, 130};   // 1.2 mm strixels
    std::vector<int>    mrReadoutSide  = {1, 1, 1};
    std::vector<double> mrLorentzAngle = {0., 0., 0.};

    std::vector<size_t>         mPhiBins   = {54, 56, 60};
    std::vector<double>         mThickness = {0.25, 0.25, 0.25};
    std::vector<Acts::Material> mMaterial
        = {ssMaterial, ssMaterial, ssMaterial};

    sslbConfig.posnegLayerBinMultipliers = {1, 2};

    sslbConfig.posnegLayerPositionsZ
        = {1220., 1500., 1800., 2150., 2550., 2950.};
    size_t nposnegs                 = sslbConfig.posnegLayerPositionsZ.size();
    sslbConfig.posnegLayerEnvelopeR = std::vector<double>(nposnegs, 5.);
    sslbConfig.posnegLayerMaterialConcentration = std::vector<int>(nposnegs, 0);
    sslbConfig.posnegLayerMaterialProperties
        = std::vector<Acts::MaterialProperties>(nposnegs, ssecmProperties);
    sslbConfig.posnegModuleMinHalfX
        = std::vector<std::vector<double>>(nposnegs, mrMinHx);
    sslbConfig.posnegModuleMaxHalfX
        = std::vector<std::vector<double>>(nposnegs, mrMaxHx);
    sslbConfig.posnegModuleHalfY
        = std::vector<std::vector<double>>(nposnegs, mrHy);
    sslbConfig.posnegModulePhiBins
        = std::vector<std::vector<size_t>>(nposnegs, mPhiBins);
    sslbConfig.posnegModuleThickness
        = std::vector<std::vector<double>>(nposnegs, mThickness);

    sslbConfig.posnegModuleReadoutBinsX
        = std::vector<std::vector<size_t>>(nposnegs, mrReadoutBinsX);
    sslbConfig.posnegModuleReadoutBinsY
        = std::vector<std::vector<size_t>>(nposnegs, mrReadoutBinsY);
    sslbConfig.posnegModuleReadoutSide
        = std::vector<std::vector<int>>(nposnegs, mrReadoutSide);
    sslbConfig.posnegModuleLorentzAngle
        = std::vector<std::vector<double>>(nposnegs, mrLorentzAngle);

    sslbConfig.posnegModuleMaterial
        = std::vector<std::vector<Acts::Material>>(nposnegs, mMaterial);

    sslbConfig.posnegModuleFrontsideStereo = {};
    sslbConfig.posnegModuleBacksideStereo  = {};
    sslbConfig.posnegModuleBacksideGap     = {};

    // mPositions
    std::vector<std::vector<std::vector<Acts::Vector3D>>>
        sslbPosnegModulePositions;
    for (size_t id = 0; id < sslbConfig.posnegLayerPositionsZ.size(); ++id) {
      sslbPosnegModulePositions.push_back(
          modulePositionsDisc(sslbConfig.posnegLayerPositionsZ[id],
                              6.0,
                              {3., 3., 3.},
                              {0., 0., 0.},
                              240.,
                              700.,
                              sslbConfig.posnegModulePhiBins[id],
                              sslbConfig.posnegModuleHalfY[id]));
    }
    sslbConfig.posnegModulePositions = sslbPosnegModulePositions;

    // define the builder
    auto sstripLayerBuilder
        = std::make_shared<const FW::Generic::LayerBuilder>(
            sslbConfig,
            Acts::getDefaultLogger("SStripLayerBuilder", layerLLevel));
    //-------------------------------------------------------------------------------------
    // build the pixel volume
    Acts::CylinderVolumeBuilder::Config ssvbConfig;
    ssvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
    ssvbConfig.volumeName           = "SStrip";
    ssvbConfig.buildToRadiusZero    = false;
    ssvbConfig.layerBuilder         = sstripLayerBuilder;
    ssvbConfig.volumeSignature      = 0;
    auto sstripVolumeBuilder
        = std::make_shared<const Acts::CylinderVolumeBuilder>(
            ssvbConfig,
            Acts::getDefaultLogger("SStripVolumeBuilder", volumeLLevel));

    //-------------------------------------------------------------------------------------
    // add to the list of builders
    volumeBuilders.push_back(sstripVolumeBuilder);

    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // LONG strip detector
    //-------------------------------------------------------------------------------------

    // configure short strip layer builder
    FW::Generic::LayerBuilder::Config lslbConfig;
    lslbConfig.layerCreator        = layerCreator;
    lslbConfig.layerIdentification = "LStrip";
    // fill necessary vectors for configuration
    //-------------------------------------------------------------------------------------
    // some prep work
    // envelope double
    std::pair<double, double> lsEnvelope(2., 2.);
    // Layer material properties - thickness, X0, L0, A, Z, Rho - barrel
    Acts::MaterialProperties lsbmProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 2.5 * Acts::units::_mm);
    Acts::MaterialProperties lsecmProperties(
        95.7, 465.2, 28.03, 14., 2.32e-3, 3.5 * Acts::units::_mm);
    // Module material - X0, L0, A, Z, Rho
    Acts::Material lsMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

    // configure the central barrel
    lslbConfig.centralLayerBinMultipliers        = {1, 1};
    lslbConfig.centralLayerRadii                 = {820., 1020.};
    lslbConfig.centralLayerEnvelopes             = {lsEnvelope, lsEnvelope};
    lslbConfig.centralLayerMaterialConcentration = {-1, -1};
    lslbConfig.centralLayerMaterialProperties
        = {lsbmProperties, lsbmProperties};
    lslbConfig.centralModuleBinningSchema = {{120, 21}, {152, 21}};
    lslbConfig.centralModuleTiltPhi       = {-0.15, -0.15};
    lslbConfig.centralModuleHalfX         = {24., 24.};
    lslbConfig.centralModuleHalfY         = {54., 54.};
    lslbConfig.centralModuleThickness     = {0.35, 0.35};
    lslbConfig.centralModuleMaterial      = {lsMaterial, lsMaterial};

    lslbConfig.centralModuleReadoutBinsX = {400, 400};  // 120 um pitch
    lslbConfig.centralModuleReadoutBinsY = {10, 10};    // 10 strips = 10.8 mm
    lslbConfig.centralModuleReadoutSide  = {1, 1};
    lslbConfig.centralModuleLorentzAngle = {0.08, 0.08};

    lslbConfig.centralModuleFrontsideStereo = {};
    lslbConfig.centralModuleBacksideStereo  = {};
    lslbConfig.centralModuleBacksideGap     = {};
    // mPositions
    std::vector<std::vector<Acts::Vector3D>> lslbCentralModulePositions;
    for (size_t lslb = 0; lslb < lslbConfig.centralLayerRadii.size(); ++lslb) {
      // call the helper function
      lslbCentralModulePositions.push_back(
          modulePositionsCylinder(lslbConfig.centralLayerRadii[lslb],
                                  3.,  // 3 mm stagger
                                  lslbConfig.centralModuleHalfY[lslb],
                                  5.,  // 5 mm module overlap
                                  lslbConfig.centralModuleBinningSchema[lslb]));
    }

    lslbConfig.centralModulePositions = lslbCentralModulePositions;

    // configure the endcaps
    mrMinHx    = {54., 66.};
    mrMaxHx    = {64.2, 72.};
    mrHy       = {78., 78.};
    mPhiBins   = {48, 50};
    mThickness = {0.35, 0.35};
    mMaterial  = {lsMaterial, lsMaterial};

    mrReadoutBinsX = {1070, 1200};  // 120 um pitch
    mrReadoutBinsY = {15, 15};      // 15 strips - 10.2 mm
    mrReadoutSide  = {1, 1};
    mrLorentzAngle = {0., 0.};

    // endcap
    lslbConfig.posnegLayerBinMultipliers = {1, 2};
    lslbConfig.posnegLayerPositionsZ
        = {1220., 1500., 1800., 2150., 2550., 2950.};
    nposnegs                        = lslbConfig.posnegLayerPositionsZ.size();
    lslbConfig.posnegLayerEnvelopeR = std::vector<double>(nposnegs, 5.);
    lslbConfig.posnegLayerMaterialConcentration = std::vector<int>(nposnegs, 0);
    lslbConfig.posnegLayerMaterialProperties
        = std::vector<Acts::MaterialProperties>(nposnegs, lsecmProperties);
    lslbConfig.posnegModuleMinHalfX
        = std::vector<std::vector<double>>(nposnegs, mrMinHx);
    lslbConfig.posnegModuleMaxHalfX
        = std::vector<std::vector<double>>(nposnegs, mrMaxHx);
    lslbConfig.posnegModuleHalfY
        = std::vector<std::vector<double>>(nposnegs, mrHy);
    lslbConfig.posnegModulePhiBins
        = std::vector<std::vector<size_t>>(nposnegs, mPhiBins);
    lslbConfig.posnegModuleThickness
        = std::vector<std::vector<double>>(nposnegs, mThickness);

    lslbConfig.posnegModuleReadoutBinsX
        = std::vector<std::vector<size_t>>(nposnegs, mrReadoutBinsX);
    lslbConfig.posnegModuleReadoutBinsY
        = std::vector<std::vector<size_t>>(nposnegs, mrReadoutBinsY);
    lslbConfig.posnegModuleReadoutSide
        = std::vector<std::vector<int>>(nposnegs, mrReadoutSide);
    lslbConfig.posnegModuleLorentzAngle
        = std::vector<std::vector<double>>(nposnegs, mrLorentzAngle);

    lslbConfig.posnegModuleMaterial
        = std::vector<std::vector<Acts::Material>>(nposnegs, mMaterial);
    lslbConfig.posnegModuleFrontsideStereo = {};
    lslbConfig.posnegModuleBacksideStereo  = {};
    lslbConfig.posnegModuleBacksideGap     = {};

    // mPositions
    std::vector<std::vector<std::vector<Acts::Vector3D>>>
        lssbPosnegModulePositions;
    for (size_t id = 0; id < lslbConfig.posnegLayerPositionsZ.size(); ++id) {
      lssbPosnegModulePositions.push_back(modulePositionsDisc(
          lslbConfig.posnegLayerPositionsZ[id],
          8.0,  // staggering of rings, we put the disk strucutre in between
          {3., 3.},
          {0., 0.},
          750.,
          1020.,
          lslbConfig.posnegModulePhiBins[id],
          lslbConfig.posnegModuleHalfY[id]));
    }
    lslbConfig.posnegModulePositions = lssbPosnegModulePositions;

    // define the builder
    auto lstripLayerBuilder
        = std::make_shared<const FW::Generic::LayerBuilder>(
            lslbConfig,
            Acts::getDefaultLogger("LStripLayerBuilder", layerLLevel));
    //-------------------------------------------------------------------------------------
    // build the pixel volume
    Acts::CylinderVolumeBuilder::Config lsvbConfig;
    lsvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
    lsvbConfig.volumeName           = "LStrip";
    lsvbConfig.buildToRadiusZero    = false;
    lsvbConfig.layerBuilder         = lstripLayerBuilder;
    lsvbConfig.volumeSignature      = 0;
    auto lstripVolumeBuilder
        = std::make_shared<const Acts::CylinderVolumeBuilder>(
            lsvbConfig,
            Acts::getDefaultLogger("LStripVolumeBuilder", volumeLLevel));
    // add to the list of builders
    volumeBuilders.push_back(lstripVolumeBuilder);
  */
    //-------------------------------------------------------------------------------------
    // create the tracking geometry
    Acts::TrackingGeometryBuilder::Config tgConfig;
    // Add the builde call functions
    for (auto& vb : volumeBuilders) {
      tgConfig.trackingVolumeBuilders.push_back(
          [=](const auto& context, const auto& inner, const auto&) {
            return vb->trackingVolume(gctx, inner);
          });
    }
    tgConfig.trackingVolumeHelper = cylinderVolumeHelper;

    auto cylinderGeometryBuilder
        = std::make_shared<const Acts::TrackingGeometryBuilder>(
            tgConfig,
            Acts::getDefaultLogger("TrackerGeometryBuilder", volumeLLevel));
    // get the geometry
    auto trackingGeometry = cylinderGeometryBuilder->trackingGeometry(gctx);
    /// return the tracking geometry
    return trackingGeometry;
  }

}  // end of namespace Generic
}  // end of namespace FW
