#include "ACTFW/Examples/DD4hepExample/DD4hepDetector.hpp"
//ACTS
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DD4hepCylinderGeometryBuilder.hpp"

//DD4Hep
#include "DD4hep/LCDD.h"

namespace DD4hepExample {
    
    std::unique_ptr<const Acts::TrackingGeometry> trackingGeometry() {
        
        DD4hep::Geometry::LCDD* dd4hepgeo = &(DD4hep::Geometry::LCDD::getInstance());
        dd4hepgeo->fromCompact("file:Examples/DD4hepExample/DD4hepDetector/compact/FCCTracker.xml");
        dd4hepgeo->volumeManager();
        dd4hepgeo->apply("DD4hepVolumeManager",0,0);
        
        //hand over LayerArrayCreator
        Acts::LayerArrayCreator::Config lacConfig;
        auto layerArrayCreator = std::make_shared<Acts::LayerArrayCreator>(lacConfig);
        //tracking volume array creator
        Acts::TrackingVolumeArrayCreator::Config tvacConfig;
        auto trackingVolumeArrayCreator = std::make_shared<Acts::TrackingVolumeArrayCreator>(tvacConfig);
        // configure the cylinder volume helper
        Acts::CylinderVolumeHelper::Config cvhConfig;
        cvhConfig.layerArrayCreator          = layerArrayCreator;
        cvhConfig.trackingVolumeArrayCreator = trackingVolumeArrayCreator;
        auto cylinderVolumeHelper = std::make_shared<Acts::CylinderVolumeHelper>(cvhConfig);
        // configure the volume builder
        Acts::CylinderVolumeBuilder::Config pvbConfig;
        pvbConfig.trackingVolumeHelper              = cylinderVolumeHelper;
        pvbConfig.volumeSignature                   = 0;
        auto cylinderVolumeBuilder = std::make_shared<Acts::CylinderVolumeBuilder>(pvbConfig);
        // configure geometry builder with the surface array creator
        Acts::DD4hepCylinderGeometryBuilder::Config cgConfig;
        cgConfig.detWorld = dd4hepgeo->world();
        cgConfig.volumeHelper = cylinderVolumeHelper;
        cgConfig.volumeBuilder = cylinderVolumeBuilder;
        auto geometryBuilder = std::make_shared<Acts::DD4hepCylinderGeometryBuilder>(cgConfig);
        // set the tracking geometry
        return  (std::move(geometryBuilder->trackingGeometry()));
    }
}

