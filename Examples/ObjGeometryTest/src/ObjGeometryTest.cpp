///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Obj/ObjTrackingGeometryWriter.hpp"

int
main()
{
    // DETECTOR:
    // --------------------------------------------------------------------------------
    // DD4Hep detector definition
    //
    // set up the geometry service 
    // DD4hepPlugin::GeometryService::Config gsConfig("GeometryService",
    //                                                Acts::Logging::INFO);
    // gsConfig.xmlFileName
    //     = "file:Examples/DD4hepExample/DD4hepDetector/compact/FCCTracker.xml";
    // auto geometrySvc = std::make_shared<DD4hepPlugin::GeometryService>(gsConfig);
    // std::shared_ptr<const Acts::TrackingGeometry> dd4Geometry
    //     = geometrySvc->trackingGeometry();
    // 
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::INFO;
    Acts::Logging::Level layerLogLevel   = Acts::Logging::INFO;
    Acts::Logging::Level volumeLogLevel  = Acts::Logging::VERBOSE;
    
    // create the tracking geometry as a shared pointer
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry
        = Acts::buildGenericDetector(
            surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);
    
    // the detectors
    std::vector<std::string> subDetectors = { "Pix", "SStrip", "LStrip" };
    // the writers
    std::vector< std::shared_ptr<FW::IWriterT<Acts::Surface> > > subWriters;
    // loop and create 
    for (auto sdet : subDetectors){
      // sub detector stream
      auto  sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
      std::string sdOutputName   = sdet+std::string(".obj");
      sdStream->open(sdOutputName);
      // object surface writers
      FWObj::ObjSurfaceWriter::Config sdObjWriterConfig(sdet);
      sdObjWriterConfig.outputPrecision = 3;
      sdObjWriterConfig.outputScalor    = 1.;
      sdObjWriterConfig.outputStream    = sdStream;
      auto sdObjWriter
          = std::make_shared<FWObj::ObjSurfaceWriter>(sdObjWriterConfig);
      // push back
      subWriters.push_back(sdObjWriter);
    }


    // configure the tracking geometry writer
    FWObj::ObjTrackingGeometryWriter::Config tgObjWriterConfig;
    tgObjWriterConfig.surfaceWriters = subWriters;
    // the tracking geometry writers 
    auto tgObjWriter
            = std::make_shared<FWObj::ObjTrackingGeometryWriter>(tgObjWriterConfig);

    // write the tracking geometry object
    tgObjWriter->write(*(tGeometry.get()));
    // -------------------------------------------------------------------------------- 
  
}
