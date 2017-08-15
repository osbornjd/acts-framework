#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ExtrapolationExampleBase.hpp"

/// The main executable
///
int
main(int argc, char* argv[])
{
  size_t nEvents = 1000;

  // get the DD4hep detector
  // DETECTOR:
  // --------------------------------------------------------------------------------
  FW::DD4hep::GeometryService::Config gsConfig("GeometryService",
                                             Acts::Logging::INFO);

  if (argc >1) {
    std::cout << "Creating detector from xml-file: '" << argv[1] << "'!"
              << std::endl;
    gsConfig.xmlFileName = argv[1];
  } else
    gsConfig.xmlFileName
        = "file:Detectors/DD4hepDetector/compact/FCChhTrackerTkLayout.xml";
  gsConfig.bTypePhi                 = Acts::equidistant;
  gsConfig.bTypeR                   = Acts::equidistant;
  gsConfig.bTypeZ                   = Acts::equidistant;
  gsConfig.envelopeR                = 0.;
  gsConfig.envelopeZ                = 0.;
  //gsConfig.buildDigitizationModules = false;

  auto geometrySvc = std::make_shared<FW::DD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
      = geometrySvc->trackingGeometry();

  // run the example
  return ACTFWExtrapolationExample::run(nEvents, dd4tGeometry);
}
