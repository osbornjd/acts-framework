// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
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
    = {Acts::Material(352.8, 407., 9.012, 4., 1.848e-3)};
auto beamPipeBuilder = std::make_shared<const Acts::PassiveLayerBuilder>(
    bplConfig,
    Acts::getDefaultLogger("BeamPipeLayerBuilder", layerLLevel));
// create the volume for the beam pipe
Acts::CylinderVolumeBuilder::Config bpvConfig;
bpvConfig.trackingVolumeHelper = cylinderVolumeHelper;
bpvConfig.volumeName           = "BeamPipe";
bpvConfig.layerBuilder         = beamPipeBuilder;
bpvConfig.layerEnvelopeR       = {1. * Acts::units::_mm, 1. * Acts::units::_mm};
bpvConfig.buildToRadiusZero    = true;
bpvConfig.volumeSignature      = 0;
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
FWGen::GenericLayerBuilder::Config plbConfig;
plbConfig.layerCreator        = layerCreator;
plbConfig.layerIdentification = "Pixel";
// fill necessary vectors for configuration
//-------------------------------------------------------------------------------------
// some prep work
// envelope for layers
std::pair<double, double> pcEnvelope(2., 2.);
// Layer material properties - thickness, X0, L0, A, Z, Rho
Acts::MaterialProperties pcmbProperties(95.7, 465.2, 28.03, 14., 2.32e-3, 1.5*Acts::units::_mm);
Acts::MaterialProperties pcmecProperties(95.7, 465.2, 28.03, 14., 2.32e-3, 1.5*Acts::units::_mm);  
// Module material - X0, L0, A, Z, Rho
Acts::Material pcMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

// standard, an approach envelope
plbConfig.approachSurfaceEnvelope = 1.;

// BARREL :
// 4 pixel layers
// configure the central barrel
plbConfig.centralLayerBinMultipliers = {1, 1};
plbConfig.centralLayerRadii          = {32., 72., 116., 172., 256. };
plbConfig.centralLayerEnvelopes
    = {pcEnvelope, pcEnvelope, pcEnvelope, pcEnvelope, pcEnvelope};
// material concentration alsways outside the modules
plbConfig.centralLayerMaterialConcentration = {1, 1, 1, 1, 1};
plbConfig.centralLayerMaterialProperties
    = {pcmbProperties, pcmbProperties, pcmbProperties, pcmbProperties, pcmbProperties};
plbConfig.centralModuleBinningSchema = {{16, 21}, {32, 21}, {52, 21}, {78, 21}, {102, 21}};
plbConfig.centralModuleTiltPhi       = {0.14, 0.14, 0.14, 0.14, 0.14 };
plbConfig.centralModuleHalfX         = {8.4, 8.4, 8.4, 8.4, 8.4};
plbConfig.centralModuleHalfY         = {36., 36., 36., 36., 36.};
plbConfig.centralModuleThickness     = {0.15, 0.15, 0.15, 0.15, 0.15};
plbConfig.centralModuleMaterial
    = {pcMaterial, pcMaterial, pcMaterial, pcMaterial, pcMaterial};
// pitch definitions
plbConfig.centralModuleReadoutBinsX = {336, 336, 336, 336, 336};
plbConfig.centralModuleReadoutBinsY = {1280, 1280, 1280, 1280, 1280};
plbConfig.centralModuleReadoutSide  = {-1, -1, -1, -1, -1};
plbConfig.centralModuleLorentzAngle = {0.12, 0.12, 0.12, 0.12, 0.12};

// no frontside/backside
plbConfig.centralModuleFrontsideStereo = {};
plbConfig.centralModuleBacksideStereo  = {};
plbConfig.centralModuleBacksideGap     = {};
// mPositions
std::vector<std::vector<Acts::Vector3D>> plbCentralModulePositions;
for (size_t plb = 0; plb < plbConfig.centralLayerRadii.size(); ++plb) {
  // call the helper function
  plbCentralModulePositions.push_back(
      modulePositionsCylinder(plbConfig.centralLayerRadii[plb],
                              0.5,  // 1 mm stagger
                              plbConfig.centralModuleHalfY[plb],
                              2.,  // 4 mm module overlap in z
                              plbConfig.centralModuleBinningSchema[plb]));
}
plbConfig.centralModulePositions = plbCentralModulePositions;

// define the builder
auto pixelLayerBuilder = std::make_shared<const FWGen::GenericLayerBuilder>(
    plbConfig,
    Acts::getDefaultLogger("PixelLayerBuilder", layerLLevel));
//-------------------------------------------------------------------------------------
// build the pixel volume
Acts::CylinderVolumeBuilder::Config pvbConfig;
pvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
pvbConfig.volumeName           = "Pixel";
pvbConfig.buildToRadiusZero    = false;
pvbConfig.layerEnvelopeR       = {1. * Acts::units::_mm, 5. * Acts::units::_mm};
pvbConfig.layerBuilder         = pixelLayerBuilder;
pvbConfig.volumeSignature      = 0;
auto pixelVolumeBuilder = std::make_shared<const Acts::CylinderVolumeBuilder>(
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
pstConfig.centralLayerRadii       = std::vector<double>(1, 300.);
pstConfig.centralLayerHalflengthZ = std::vector<double>(1, 1800.);
pstConfig.centralLayerThickness   = std::vector<double>(1, 1.8);
pstConfig.centralLayerMaterial
    = {Acts::Material(352.8, 407., 9.012, 4., 1.848e-3)};
auto pstBuilder = std::make_shared<const Acts::PassiveLayerBuilder>(
    pstConfig,
    Acts::getDefaultLogger("PSTBuilder", layerLLevel));
// create the volume for the beam pipe
Acts::CylinderVolumeBuilder::Config pstvolConfig;
pstvolConfig.trackingVolumeHelper = cylinderVolumeHelper;
pstvolConfig.volumeName           = "PST";
pstvolConfig.buildToRadiusZero    = false;
pstvolConfig.layerBuilder         = pstBuilder;
pstvolConfig.volumeSignature      = 0;
auto pstVolumeBuilder = std::make_shared<const Acts::CylinderVolumeBuilder>(
    pstvolConfig,
    Acts::getDefaultLogger("PSTVolumeBuilder", volumeLLevel));
// add to the detector builds
volumeBuilders.push_back(pstVolumeBuilder);

//-------------------------------------------------------------------------------------
// SHORT strip detector
//-------------------------------------------------------------------------------------
// first add a Pixel Support Tube
// STRIPS
// ----------------------------------------------------------------------------
// configure short strip layer builder
FWGen::GenericLayerBuilder::Config sslbConfig;
sslbConfig.layerCreator        = layerCreator;
sslbConfig.layerIdentification = "SStrip";
// fill necessary vectors for configuration
//-------------------------------------------------------------------------------------
// some prep work
// envelope double
std::pair<double, double> ssEnvelope(2., 2.);
// Layer material properties - thickness, X0, L0, A, Z, Rho
Acts::MaterialProperties ssbmProperties(95.7, 465.2, 28.03, 14., 2.32e-3, 2.*Acts::units::_mm);
Acts::MaterialProperties ssecmProperties(95.7, 465.2, 28.03, 14., 2.32e-3, 2.5*Acts::units::_mm);

// Module material - X0, L0, A, Z, Rho
Acts::Material ssMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

// configure the central barrel
sslbConfig.centralLayerBinMultipliers        = {1, 1};
sslbConfig.centralLayerRadii                 = {420., 600.};
sslbConfig.centralLayerEnvelopes             = {ssEnvelope, ssEnvelope };
sslbConfig.centralLayerMaterialConcentration = {-1, -1};
sslbConfig.centralLayerMaterialProperties    = {ssbmProperties, ssbmProperties };
sslbConfig.centralModuleBinningSchema        = {{52, 21}, {102, 21}};
sslbConfig.centralModuleTiltPhi              = {-0.15, -0.15};
sslbConfig.centralModuleHalfX                = {24., 24. };
sslbConfig.centralModuleHalfY                = {54., 54. };
sslbConfig.centralModuleThickness            = {0.25, 0.25};

sslbConfig.centralModuleReadoutBinsX = {600, 600 };  // 80 um pitch
sslbConfig.centralModuleReadoutBinsY = {90, 90};      // 1.2 mm strixels
sslbConfig.centralModuleReadoutSide  = {1, 1};
sslbConfig.centralModuleLorentzAngle = {0.12, 0.12};

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

// define the builder
auto sstripLayerBuilder = std::make_shared<const FWGen::GenericLayerBuilder>(
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
auto sstripVolumeBuilder = std::make_shared<const Acts::CylinderVolumeBuilder>(
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
FWGen::GenericLayerBuilder::Config lslbConfig;
lslbConfig.layerCreator        = layerCreator;
lslbConfig.layerIdentification = "LStrip";
// fill necessary vectors for configuration
//-------------------------------------------------------------------------------------
// some prep work
// envelope double
std::pair<double, double> lsEnvelope(2., 2.);
// Layer material properties - thickness, X0, L0, A, Z, Rho - barrel
Acts::MaterialProperties lsbmProperties(95.7, 465.2, 28.03, 14., 2.32e-3, 2.5*Acts::units::_mm);
Acts::MaterialProperties lsecmProperties( 95.7, 465.2, 28.03, 14., 2.32e-3, 3.5*Acts::units::_mm);
// Module material - X0, L0, A, Z, Rho
Acts::Material lsMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

// configure the central barrel
lslbConfig.centralLayerBinMultipliers        = {1, 1};
lslbConfig.centralLayerRadii                 = {780., 1020.};
lslbConfig.centralLayerEnvelopes             = {lsEnvelope, lsEnvelope};
lslbConfig.centralLayerMaterialConcentration = {-1, -1};
lslbConfig.centralLayerMaterialProperties    = {lsbmProperties, lsbmProperties};
lslbConfig.centralModuleBinningSchema        = {{120, 21}, {152, 21}};
lslbConfig.centralModuleTiltPhi              = {-0.15, -0.15};
lslbConfig.centralModuleHalfX                = {24., 24.};
lslbConfig.centralModuleHalfY                = {54., 54.};
lslbConfig.centralModuleThickness            = {0.35, 0.35};
lslbConfig.centralModuleMaterial             = {lsMaterial, lsMaterial};

lslbConfig.centralModuleReadoutBinsX = {400, 400};  // 120 um pitch
lslbConfig.centralModuleReadoutBinsY = {1, 1};      // 10 strips = 10.8 mm
lslbConfig.centralModuleReadoutSide  = {1, 1};
lslbConfig.centralModuleLorentzAngle = {0.08, 0.08};

lslbConfig.centralModuleFrontsideStereo = { -0.02, -0.02 };
lslbConfig.centralModuleBacksideStereo  = { 0.02, 0.02 };
lslbConfig.centralModuleBacksideGap     = { 2., 2.};
// mPositions
std::vector<std::vector<Acts::Vector3D>> lslbCentralModulePositions;
for (size_t lslb = 0; lslb < lslbConfig.centralLayerRadii.size(); ++lslb) {
  // call the helper function
  lslbCentralModulePositions.push_back(
      modulePositionsCylinder(lslbConfig.centralLayerRadii[lslb],
                              8.,  // 10 mm stagger
                              lslbConfig.centralModuleHalfY[lslb],
                              5.,  // 5 mm module overlap
                              lslbConfig.centralModuleBinningSchema[lslb]));
}

lslbConfig.centralModulePositions = lslbCentralModulePositions;

// define the builder
auto lstripLayerBuilder = std::make_shared<const FWGen::GenericLayerBuilder>(
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
auto lstripVolumeBuilder = std::make_shared<const Acts::CylinderVolumeBuilder>(
    lsvbConfig,
    Acts::getDefaultLogger("LStripVolumeBuilder", volumeLLevel));
// add to the list of builders
volumeBuilders.push_back(lstripVolumeBuilder);

