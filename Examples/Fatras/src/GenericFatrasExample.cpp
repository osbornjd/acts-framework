#include "FatrasExampleBase.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"

// the main executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 1000;
  
  // get the generic detector
  // DETECTOR:
  // --------------------------------------------------------------------------------
  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel = Acts::Logging::VERBOSE;
  Acts::Logging::Level layerLogLevel   = Acts::Logging::VERBOSE;
  Acts::Logging::Level volumeLogLevel  = Acts::Logging::VERBOSE;
  
  // create the tracking geometry as a shared pointer
  std::shared_ptr<const Acts::TrackingGeometry> gtGeometry
      = FWGen::buildGenericDetector(
          surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);

  // run the example
  return ACTFWFatrasExample::run(nEvents,gtGeometry);
}
