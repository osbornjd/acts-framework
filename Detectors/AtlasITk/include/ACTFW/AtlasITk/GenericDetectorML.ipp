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
bplConfig.centralLayerRadii       = std::vector<double>(1, 19. * Acts::units::_mm);
bplConfig.centralLayerHalflengthZ = std::vector<double>(1, 3000. * Acts::units::_mm);
bplConfig.centralLayerThickness   = std::vector<double>(1, 0.8 * Acts::units::_mm);
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
std::pair<double, double> pcEnvelope(2. * Acts::units::_mm, 2. * Acts::units::_mm);
// Layer material properties - thickness, X0, L0, A, Z, Rho
Acts::MaterialProperties pcmProperties(1., 95.7, 465.2, 28.03, 14., 2.32e-3);
// Module material - X0, L0, A, Z, Rho
Acts::Material pcMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

// standard, an approach envelope
plbConfig.approachSurfaceEnvelope = 1.;

// BARREL :
// 3 pixel layers
// configure the central barrel
plbConfig.centralLayerBinMultipliers = {1, 1};
plbConfig.centralLayerRadii          = {50.5 * Acts::units::_mm, 88.5 * Acts::units::_mm, 122.5 * Acts::units::_mm};
plbConfig.centralLayerEnvelopes
    = {pcEnvelope, pcEnvelope, pcEnvelope};
// material concentration alsways outside the modules
plbConfig.centralLayerMaterialConcentration = {1, 1, 1};
plbConfig.centralLayerMaterialProperties
    = {pcmProperties, pcmProperties, pcmProperties};
plbConfig.centralModuleBinningSchema = {{22, 13}, {38, 13}, {52, 13}};
plbConfig.centralModuleTiltPhi       = {0.35, 0.35, 0.35};
plbConfig.centralModuleHalfX         = {1.64 / 2 * Acts::units::_cm, 1.64 / 2 * Acts::units::_cm, 1.64 / 2 * Acts::units::_cm};
plbConfig.centralModuleHalfY         = {6.16 / 2 * Acts::units::_cm, 6.16 / 2 * Acts::units::_cm, 6.16 / 2 * Acts::units::_cm}; // 6.08 cm active
plbConfig.centralModuleThickness     = {256. * Acts::units::_um, 256. * Acts::units::_um, 256. * Acts::units::_um};
plbConfig.centralModuleMaterial
    = {pcMaterial, pcMaterial, pcMaterial};
// pitch definitions
plbConfig.centralModuleReadoutBinsX = {328, 328, 328};
plbConfig.centralModuleReadoutBinsY = {144, 144, 144};
plbConfig.centralModuleReadoutSide  = {-1, -1, -1};
plbConfig.centralModuleLorentzAngle = {0.12, 0.12, 0.12};

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

// ENDCAP :
// 3 pixel layers
// configure the endcaps
plbConfig.posnegLayerBinMultipliers = {1, 1};

plbConfig.posnegLayerPositionsZ = {495. * Acts::units::_mm,
                                   580. * Acts::units::_mm,
                                   650. * Acts::units::_mm};

plbConfig.posnegLayerEnvelopeR = {1. * Acts::units::_mm,
                                  1. * Acts::units::_mm,
                                  1. * Acts::units::_mm};
// material concentration is always behing
plbConfig.posnegLayerMaterialConcentration = std::vector<int>(6, 0);
// layer structure surface has pixel material properties
plbConfig.posnegLayerMaterialProperties = {pcmProperties,
                                           pcmProperties,
                                           pcmProperties,
                                           pcmProperties,
                                           pcmProperties,
                                           pcmProperties};
std::vector<double>         perHX = {34.87 * Acts::units::_mm, 34.87 * Acts::units::_mm};    // half length x
std::vector<double>         perHY = {30.4 * Acts::units::_mm, 30.4 * Acts::units::_mm};    // half length y
std::vector<size_t>         perBP = {8, 8};      // bins in phi
std::vector<double>         perT  = {0.15, 0.15};  // module thickness
std::vector<size_t>         perBX = {144, 144};    // bins in x
std::vector<size_t>         perBY = {328, 328};  // bins in y
std::vector<int>            perRS = {-1, -1};      // readout side
std::vector<double>         perLA = {0., 0.};      // lorentz angle
std::vector<Acts::Material> perM  = {pcMaterial, pcMaterial};  // material

plbConfig.posnegModuleMinHalfX     = std::vector<std::vector<double>>(3, perHX);
plbConfig.posnegModuleMaxHalfX     = {};
plbConfig.posnegModuleHalfY        = std::vector<std::vector<double>>(3, perHY);
plbConfig.posnegModulePhiBins      = std::vector<std::vector<size_t>>(3, perBP);
plbConfig.posnegModuleThickness    = std::vector<std::vector<double>>(3, perT);
plbConfig.posnegModuleReadoutBinsX = std::vector<std::vector<size_t>>(3, perBX);
plbConfig.posnegModuleReadoutBinsY = std::vector<std::vector<size_t>>(3, perBY);
plbConfig.posnegModuleReadoutSide  = std::vector<std::vector<int>>(3, perRS);
plbConfig.posnegModuleLorentzAngle = std::vector<std::vector<double>>(3, perLA);
plbConfig.posnegModuleMaterial
    = std::vector<std::vector<Acts::Material>>(3, perM);

// no frontside/backside
plbConfig.posnegModuleFrontsideStereo = {};
plbConfig.posnegModuleBacksideStereo  = {};
plbConfig.posnegModuleBacksideGap     = {};
// mPositions
std::vector<std::vector<std::vector<Acts::Vector3D>>> plbPosnegModulePositions;
for (size_t id = 0; id < plbConfig.posnegLayerPositionsZ.size(); ++id) {
  plbPosnegModulePositions.push_back(
      modulePositionsDisc(plbConfig.posnegLayerPositionsZ[id],
                          0.0,
                          {4.0, 4.0},
                          {0.5, 0.},
                          88.8 * Acts::units::_mm,
                          149.6 * Acts::units::_mm,
                          plbConfig.posnegModulePhiBins[id],
                          plbConfig.posnegModuleHalfY[id]));
}
plbConfig.posnegModulePositions = plbPosnegModulePositions;

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
pstConfig.centralLayerRadii       = std::vector<double>(1, 200.);
pstConfig.centralLayerHalflengthZ = std::vector<double>(1, 2800.);
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
Acts::MaterialProperties ssmProperties(1., 95.7, 465.2, 28.03, 14., 2.32e-3);
// Module material - X0, L0, A, Z, Rho
Acts::Material ssMaterial(95.7, 465.2, 28.03, 14., 2.32e-3);

// configure the central barrel
sslbConfig.centralLayerBinMultipliers = {1, 1};
sslbConfig.centralLayerRadii          = {299. * Acts::units::_mm, 371. * Acts::units::_mm, 443. * Acts::units::_mm, 514. * Acts::units::_mm};
sslbConfig.centralLayerEnvelopes
    = {ssEnvelope, ssEnvelope, ssEnvelope, ssEnvelope};
sslbConfig.centralLayerMaterialConcentration = {-1, -1, -1, -1};
sslbConfig.centralLayerMaterialProperties
    = {ssmProperties, ssmProperties, ssmProperties, ssmProperties};
sslbConfig.centralModuleBinningSchema
    = {{32, 12}, {40, 12}, {48, 12}, {56, 12}};
sslbConfig.centralModuleTiltPhi   = {-0.19, -0.19, -0.2, -0.2};
sslbConfig.centralModuleHalfX     = {58.27 / 2 * Acts::units::_mm, 58.27 / 2 * Acts::units::_mm, 58.27 / 2 * Acts::units::_mm, 58.27 / 2 * Acts::units::_mm};
sslbConfig.centralModuleHalfY     = {128.18 / 2 * Acts::units::_mm, 128.18 / 2 * Acts::units::_mm, 128.18 / 2 * Acts::units::_mm, 128.18 / 2 * Acts::units::_mm};
sslbConfig.centralModuleThickness = {0.25, 0.25, 0.25, 0.25};

sslbConfig.centralModuleReadoutBinsX = {729, 729, 729, 729};  // 80 um pitch
sslbConfig.centralModuleReadoutBinsY = {2, 2, 2, 2};          
sslbConfig.centralModuleReadoutSide  = {1, 1, 1, 1};
sslbConfig.centralModuleLorentzAngle = {0.12, 0.12, 0.12, 0.12};

sslbConfig.centralModuleMaterial
    = {ssMaterial, ssMaterial, ssMaterial, ssMaterial};
sslbConfig.centralModuleFrontsideStereo = {-0.02, -0.02, -0.02, -0.02};
sslbConfig.centralModuleBacksideStereo  = {0.02, 0.02, 0.02, 0.02};
sslbConfig.centralModuleBacksideGap     = {2., 2., 2., 2.};
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
std::vector<double> mrMinHx = {43.2 / 2, 53.03 / 2, 68.91 / 2};
std::vector<double> mrMaxHx = {52.48 / 2, 69.95 / 2, 87.96 / 2};
std::vector<double> mrHy    = {59.1 / 2, 117.7 / 2, 122.3 / 2};

// simplified strixels readout
std::vector<size_t> mrReadoutBinsX = {729, 729, 729};  // 50 um pitch
std::vector<size_t> mrReadoutBinsY = {1, 2, 2};      
std::vector<int>    mrReadoutSide  = {1, 1, 1};
std::vector<double> mrLorentzAngle = {0., 0., 0.};

std::vector<size_t>         mPhiBins   = {40, 40, 52};
std::vector<double>         mThickness = {0.380, 0.380, 0.380};
std::vector<Acts::Material> mMaterial  = {ssMaterial, ssMaterial, ssMaterial};

sslbConfig.posnegLayerBinMultipliers = {1, 1};

sslbConfig.posnegLayerPositionsZ = {853.8, 934., 1091.5, 1299.9, 1399.7, 1771.4, 2115.2, 2505., 2720.2};
size_t nposnegs                  = sslbConfig.posnegLayerPositionsZ.size();
sslbConfig.posnegLayerEnvelopeR  = std::vector<double>(nposnegs, 5);
sslbConfig.posnegLayerMaterialConcentration = std::vector<int>(nposnegs, 0);
sslbConfig.posnegLayerMaterialProperties
    = std::vector<Acts::MaterialProperties>(nposnegs, ssmProperties);
sslbConfig.posnegModuleMinHalfX
    = std::vector<std::vector<double>>(nposnegs, mrMinHx);
sslbConfig.posnegModuleMaxHalfX
    = std::vector<std::vector<double>>(nposnegs, mrMaxHx);
sslbConfig.posnegModuleHalfY = std::vector<std::vector<double>>(nposnegs, mrHy);
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

sslbConfig.posnegModuleFrontsideStereo = std::vector<std::vector<double>>(nposnegs, {-0.02, -0.02, -0.02});
sslbConfig.posnegModuleBacksideStereo  = std::vector<std::vector<double>>(nposnegs, {0.02, 0.02, 0.02});
sslbConfig.posnegModuleBacksideGap     = std::vector<std::vector<double>>(nposnegs, {1., 1., 1.});


// mPositions
std::vector<std::vector<std::vector<Acts::Vector3D>>> sslbPosnegModulePositions;

//std::vector<double> rMin = {275., 337.6, 337.6, 337.6, 337.6, 337.6, 337.6, 337.6, 438.7};
//std::vector<double> rMax = {334.1, 455.3, 455.3, 455.3, 455.3, 455.3, 455.3, 455.3, 560.};

for (size_t id = 0; id < sslbConfig.posnegLayerPositionsZ.size(); ++id) {
  sslbPosnegModulePositions.push_back(
      modulePositionsDisc(sslbConfig.posnegLayerPositionsZ[id],
                          10.0,
                          {3., 3., 3.},
                          {0., 0., 0.},
                          275.,
                          560.,
                          sslbConfig.posnegModulePhiBins[id],
                          sslbConfig.posnegModuleHalfY[id]));
}
sslbConfig.posnegModulePositions = sslbPosnegModulePositions;

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
