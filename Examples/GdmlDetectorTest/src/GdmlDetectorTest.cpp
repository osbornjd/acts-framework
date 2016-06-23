//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#include <stdio.h>
#include <memory>
// ACTS inlcude
//#include "ACTS/Plugins/TGeoPlugins/TGeoGdmlCylinderGeometryBuilder.hpp"
//#include "ACTS/Plugins/Geant4Plugins/GdmlCylinderGeometryBuilder.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"

// the main hello world executable
int main (int argc, char *argv[])
{

    std::shared_ptr<Acts::TrackingGeometry> tGeometry = nullptr;

    //
    //bool buildROOTGDML = true;
    //if (!buildROOTGDML){
    //    // build from G4 Gdml
    //    Acts::GdmlCylinderGeometryBuilder::Config gdmlGbuilderConfig;
    //    gdmlGbuilderConfig.gdmlFile = "/Users/salzburg/Desktop/Pixel.gdml";
   //
   //     auto cylGeometryBuilder = std::make_shared<Acts::GdmlCylinderGeometryBuilder>(gdmlGbuilderConfig);
   //     tGeometry = std::move(cylGeometryBuilder->trackingGeometry());
   // } else {
   //
   //     //
   //     Acts::TGeoGdmlCylinderGeometryBuilder::LayerConfig pixell0Config;
   //     pixell0Config.layerName  = "Pixel::Layer0";
   //     pixell0Config.sensorName = "siBlay";
   //
   //     Acts::TGeoGdmlCylinderGeometryBuilder::VolumeConfig pixelBarrelConfig;
   //     pixelBarrelConfig.volumeName = "Pixel::Barrel";
   //     pixelBarrelConfig.layerConfigs.push_back(pixell0Config);
   //
   //     // build from ROOT Gdml - into TGeo model
   //     Acts::TGeoGdmlCylinderGeometryBuilder::Config gdmlGbuilderConfig;
   //     gdmlGbuilderConfig.gdmlFile = "/Users/salzburg/Desktop/Pixel.gdml";
   //     gdmlGbuilderConfig.volumeConfigs.push_back(pixelBarrelConfig);
   //
   //     auto cylGeometryBuilder = std::make_shared<Acts::TGeoGdmlCylinderGeometryBuilder>(gdmlGbuilderConfig);
   //     tGeometry = std::move(cylGeometryBuilder->trackingGeometry());
   //
   //
   //
   // }
    
    return 0;
}
