///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"

int
main(int argc, char* argv[])
{
  // DETECTOR:
  // --------------------------------------------------------------------------------
  // DD4Hep detector definition
  //
  // set up the geometry service
  FWDD4hep::GeometryService::Config gsConfig("GeometryService",
                                             Acts::Logging::INFO);
  if (argc > 1) {
    std::cout << "Creating detector from xml-file: '" << argv[1] << "'!"
              << std::endl;
    gsConfig.xmlFileName = argv[1];
  } else
    gsConfig.xmlFileName
        = "file:Detectors/DD4hepDetector/compact/FCChhTrackerTkLayout.xml";
  gsConfig.bTypePhi  = Acts::equidistant;
  gsConfig.bTypeR    = Acts::equidistant;
  gsConfig.bTypeZ    = Acts::equidistant;
  gsConfig.envelopeR = 0.;
  gsConfig.envelopeZ = 0.;

  auto geometrySvc = std::make_shared<FWDD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4Geometry
      = geometrySvc->trackingGeometry();

  // the detectors
  std::vector<std::string> subDetectors
      = {"FCChhBeampipe", "FCChhInner0", "FCChhInner", "FCChhOuter"};
  // the writers
  std::vector<std::shared_ptr<FW::IWriterT<Acts::Surface>>> subWriters;
  std::vector<std::shared_ptr<std::ofstream>>               subStreams;
  // loop and create
  for (auto sdet : subDetectors) {
    // sub detector stream
    auto        sdStream = std::shared_ptr<std::ofstream>(new std::ofstream);
    std::string sdOutputName = sdet + std::string(".obj");
    sdStream->open(sdOutputName);
    // object surface writers
    FWObj::ObjSurfaceWriter::Config sdObjWriterConfig(sdet,
                                                      Acts::Logging::INFO);
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
  FWObj::ObjTrackingGeometryWriter::Config tgObjWriterConfig(
      "ObjTrackingGeometryWriter", Acts::Logging::VERBOSE);
  tgObjWriterConfig.surfaceWriters       = subWriters;
  tgObjWriterConfig.filePrefix           = "mtllib materials.mtl";
  tgObjWriterConfig.sensitiveGroupPrefix = "usemtl silicon'\n'";
  tgObjWriterConfig.layerPrefix          = "usemtl support'\n'";
  // the tracking geometry writers
  auto tgObjWriter
      = std::make_shared<FWObj::ObjTrackingGeometryWriter>(tgObjWriterConfig);

  // write the tracking geometry object
  tgObjWriter->write(*(dd4Geometry.get()));

  // --------------------------------------------------------------------------------
  // close the output streams
  for (auto sStreams : subStreams) {
    sStreams->close();
  }
}
