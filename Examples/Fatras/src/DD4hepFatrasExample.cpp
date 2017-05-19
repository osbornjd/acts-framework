#include "FatrasExampleBase.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"

// the main executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 1000;
  
  // get the DD4hep detector
  // DETECTOR:
  // --------------------------------------------------------------------------------
  FWDD4hep::GeometryService::Config gsConfig("GeometryService",
                                              Acts::Logging::INFO);
  
  gsConfig.xmlFileName
       = "file:Examples/DD4hepExample/DD4hepDetector/compact/FCChhTrackerTkLayout.xml";
  auto geometrySvc = std::make_shared<FWDD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
       = geometrySvc->trackingGeometry();
  
  // run the example
  return ACTFWFatrasExample::run(nEvents,dd4tGeometry);
}
