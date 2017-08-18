///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"

int
main()
{
    // DETECTOR:
    // --------------------------------------------------------------------------------
    // set geometry building logging level
    Acts::Logging::Level surfaceLogLevel = Acts::Logging::DEBUG;
    Acts::Logging::Level layerLogLevel   = Acts::Logging::DEBUG;
    Acts::Logging::Level volumeLogLevel  = Acts::Logging::VERBOSE;
    
    // create the tracking geometry as a shared pointer
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry
        = FWGen::buildGenericDetector(
            surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);
    
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
                                                              Acts::Logging::INFO);
    tgObjWriterConfig.surfaceWriters = subWriters;
    tgObjWriterConfig.filePrefix           = "mtllib materials.mtl\n";
    tgObjWriterConfig.sensitiveGroupPrefix = "usemtl silicon\n";
    tgObjWriterConfig.layerPrefix          = "usemtl support\n";
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
