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

    // the detectors
    std::vector<std::string> subDetectors = { "BeamPipe", "Pix", "PST", "SStrip", "LStrip" };
    // the writers
    std::vector< std::shared_ptr<FW::IWriterT<Acts::Surface> > > subWriters;
    std::vector< std::shared_ptr<std::ofstream> > subStreams;
    // loop and create
    for (auto sdet : subDetectors){
      // sub detector stream
      auto  sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
      std::string sdOutputName   = sdet+std::string(".obj");
      sdStream->open(sdOutputName);
      // object surface writers
      FWObj::ObjSurfaceWriter::Config sdObjWriterConfig(sdet, Acts::Logging::INFO);
      sdObjWriterConfig.filePrefix         = "mtllib materials.mtl";
      sdObjWriterConfig.outputPhiSegemnts  = 72;
      sdObjWriterConfig.outputPrecision    = 6;
      sdObjWriterConfig.outputScalor       = 1.;
      sdObjWriterConfig.outputThickness    = 1.;
      sdObjWriterConfig.outputSensitive    = true;
      sdObjWriterConfig.outputLayerSurface = true;
      sdObjWriterConfig.outputStream       = sdStream;
      auto sdObjWriter
          = std::make_shared<FWObj::ObjSurfaceWriter>(sdObjWriterConfig);
      // call initialize      
      sdObjWriter->initialize();
      // push back
      subWriters.push_back(sdObjWriter);
      subStreams.push_back(sdStream);
    }
    // configure the tracking geometry writer
    FWObj::ObjTrackingGeometryWriter::Config tgObjWriterConfig("ObjTrackingGeometryWriter",
                                                              Acts::Logging::VERBOSE);
    tgObjWriterConfig.surfaceWriters = subWriters;
    tgObjWriterConfig.filePrefix           = "mtllib materials.mtl";
    tgObjWriterConfig.sensitiveGroupPrefix = "usemtl silicon";
    tgObjWriterConfig.layerPrefix          = "usemtl support";
    // the tracking geometry writers
    auto tgObjWriter
            = std::make_shared<FWObj::ObjTrackingGeometryWriter>(tgObjWriterConfig);

    // write the tracking geometry object
    tgObjWriter->write(*(tGeometry.get()));
  
    // -------------------------------------------------------------------------------- 
    // close the output streams
    for (auto sStreams : subStreams){
      sStreams->close();
    }
  
}
