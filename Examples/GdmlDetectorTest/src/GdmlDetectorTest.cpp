#include <cstdlib>
#include <memory>
#include <array>
#include <iostream>
#include <vector>

#include <TGeoManager.h>
#include <TApplication.h>

#include "ACTS/Plugins/TGeoPlugins/TGeoLayerBuilder.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Material/Material.hpp"
#include "ACTS/Tools/CylinderGeometryBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/LayerCreator.hpp"
#include "ACTS/Tools/PassiveLayerBuilder.hpp"
#include "ACTS/Tools/SurfaceArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"

std::unique_ptr<const Acts::TrackingGeometry> gdmlTrackingGeometry(Acts::Logging::Level lvl = Acts::Logging::VERBOSE)
{
  auto makeLogger = [=](const std::string& name) {
    return Acts::getDefaultLogger(name, lvl);
  };

  auto surfaceArrayCreator = std::make_shared<Acts::SurfaceArrayCreator>(makeLogger("SurfaceArrayCreator"));
  // configure the layer creator that uses the surface array creator
  Acts::LayerCreator::Config lcConfig;
  lcConfig.surfaceArrayCreator = surfaceArrayCreator;
  auto layerCreator = std::make_shared<Acts::LayerCreator>(lcConfig, makeLogger("LayerCreator"));
  auto layerArrayCreator = std::make_shared<Acts::LayerArrayCreator>(makeLogger("LayerArrayCreator"));
  auto tVolumeArrayCreator = std::make_shared<Acts::TrackingVolumeArrayCreator>(makeLogger("TrackingVolumeArrayCreator"));
  // configure the cylinder volume helper
  Acts::CylinderVolumeHelper::Config cvhConfig;
  cvhConfig.layerArrayCreator = layerArrayCreator;
  cvhConfig.trackingVolumeArrayCreator = tVolumeArrayCreator;
  auto cylinderVolumeHelper
      = std::make_shared<Acts::CylinderVolumeHelper>(cvhConfig, makeLogger("CylinderVolumeHelper"));
  
  //-------------------------------------------------------------------------------------
  // beam pipe
  //-------------------------------------------------------------------------------------
  // configure the beam pipe layer builder
  Acts::PassiveLayerBuilder::Config bplConfig;
  bplConfig.layerIdentification     = "BeamPipe";
  bplConfig.centralLayerRadii       = std::vector<double>(1, 19.);
  bplConfig.centralLayerHalflengthZ = std::vector<double>(1, 200.);
  bplConfig.centralLayerThickness   = std::vector<double>(1, 0.8);
  bplConfig.centralLayerMaterial    = { Acts::Material(352.8,407.,9.012, 4., 1.848e-3) };
  auto beamPipeBuilder = std::make_shared<Acts::PassiveLayerBuilder>(bplConfig, makeLogger("PassiveLayerBuilder"));
  // create the volume for the beam pipe
  Acts::CylinderVolumeBuilder::Config bpvConfig;
  bpvConfig.trackingVolumeHelper = cylinderVolumeHelper;
  bpvConfig.volumeName           = "BeamPipe";
  bpvConfig.layerBuilder         = beamPipeBuilder;
  bpvConfig.layerEnvelopeR       = 1.;
  bpvConfig.layerEnvelopeZ       = 1.;
  bpvConfig.volumeSignature      = 0;
  auto beamPipeVolumeBuilder
      = std::make_shared<Acts::CylinderVolumeBuilder>(bpvConfig, makeLogger("CylinderVolumeBuilder"));
  
  // ATLAS pixel detector
  TGeoManager::Import("Pixel_ATLAS.gdml");
  //
  // configuration for Layer0
  Acts::TGeoLayerBuilder::LayerConfig pix0Config;
  pix0Config.layerName  = "Pixel::Layer0";
  pix0Config.sensorName = "SensorBrl";
  pix0Config.envelope   = std::pair<double,double>(1.,5.);
  pix0Config.localAxes  = "yzx";
  pix0Config.binsLoc0   = 40;
  pix0Config.binsLoc1   = 40;
  //
  // configuration for Layer1
  Acts::TGeoLayerBuilder::LayerConfig pix1Config;
  pix1Config.layerName  = "Pixel::Layer1";
  pix1Config.sensorName = "SensorBrl";
  pix1Config.envelope   = std::pair<double,double>(1.,5.);
  pix1Config.localAxes  = "yzx";
  pix1Config.binsLoc0   = 40;
  pix1Config.binsLoc1   = 40;
  //
  // configuration for Layer2
  Acts::TGeoLayerBuilder::LayerConfig pix2Config;
  pix2Config.layerName  = "Pixel::Layer2";
  pix2Config.sensorName = "SensorBrl";
  pix2Config.envelope   = std::pair<double,double>(1.,5.);
  pix2Config.localAxes  = "yzx";
  pix2Config.binsLoc0   = 40;
  pix2Config.binsLoc1   = 40;
  // configuration for the layer builder
  Acts::TGeoLayerBuilder::Config pixLayerBuilderConfig;
  pixLayerBuilderConfig.configurationName    = "Pixel";
  pixLayerBuilderConfig.unit                 = 10.;
  pixLayerBuilderConfig.layerCreator         = layerCreator;
  pixLayerBuilderConfig.negativeLayerConfigs = {};
    pixLayerBuilderConfig.centralLayerConfigs  = { pix0Config }; //, pix1Config, pix2Config };
  pixLayerBuilderConfig.positiveLayerConfigs = {};
  ///
  auto pixelLayerBuilder = std::make_shared<Acts::TGeoLayerBuilder>(pixLayerBuilderConfig, makeLogger("PixelLayerBuilder"));
    
  /// create the geometry
  //-------------------------------------------------------------------------------------
  // build the pixel volume
  Acts::CylinderVolumeBuilder::Config pvbConfig;
  pvbConfig.trackingVolumeHelper = cylinderVolumeHelper;
  pvbConfig.volumeName           = "Pixel";
  pvbConfig.volumeToBeamPipe     = false;
  pvbConfig.layerBuilder         = pixelLayerBuilder;
  pvbConfig.layerEnvelopeR       = 1.;
  pvbConfig.layerEnvelopeZ       = 10.;
  pvbConfig.volumeSignature      = 0;
  auto pixelVolumeBuilder
  = std::make_shared<Acts::CylinderVolumeBuilder>(pvbConfig, makeLogger("PixelVolumeBuilder"));

  //-------------------------------------------------------------------------------------
  // list the volume builders
  std::list< std::shared_ptr<Acts::ITrackingVolumeBuilder> > detectorBuilders;
  detectorBuilders.push_back(pixelVolumeBuilder);
  
  //-------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------
  // create the tracking geometry
  Acts::CylinderGeometryBuilder::Config tgConfig;
  tgConfig.beamPipeBuilder        = beamPipeVolumeBuilder;
  tgConfig.trackingVolumeBuilders = detectorBuilders;
  tgConfig.trackingVolumeHelper   = cylinderVolumeHelper;
  // cylindrical geometry builder
  auto cylinderGeometryBuilder
  = std::make_shared<const Acts::CylinderGeometryBuilder>(tgConfig, makeLogger("CylinderGeometryBuilder"));
  return cylinderGeometryBuilder->trackingGeometry();
}


// the main hello world executable
int main (int argc, char *argv[])
{
    
    TApplication theApp("tapp", &argc, argv);

    std::shared_ptr<const Acts::TrackingGeometry> tGeometry =
    std::move(gdmlTrackingGeometry(Acts::Logging::DEBUG));
    
    gGeoManager->GetTopVolume()->Draw("ogl");

    theApp.Run();
    
    return 0;
}
