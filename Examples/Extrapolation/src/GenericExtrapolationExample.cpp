#include <boost/program_options.hpp>
#include "ExtrapolationExampleBase.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Plugins/BField/BFieldFromFile.hpp"

namespace po = boost::program_options;

// the main executable
int
main(int argc, char* argv[])
{  
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
      "events,n",
      po::value<size_t>()->default_value(1000),
      "The number of events to be processed")(
      "loglevel,l",
      po::value<size_t>()->default_value(2),
      "The output log level.");
  FW::BField::bFieldOptions<po::options_description>(desc);    
  
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  
  // output messages
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // create BField service
  auto bField = FW::BField::bFieldFromFile<po::variables_map>(vm);
  
  size_t nEvents = 1000;
  if (vm.count("events")){
    nEvents = vm["events"].as<size_t>();
    std::cout << "- running " << nEvents << " events "<< std::endl;
  } else {
    std::cout << "- using standard number of events " << nEvents << std::endl;
  }
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;  
  if (vm.count("loglevel")) {
    logLevel =   Acts::Logging::Level(vm["loglevel"].as<size_t>());    
    std::cout << "- the output log level is set to " << logLevel << std::endl;
  } else {
    std::cout << "- default log level is " << logLevel << std::endl;
  }
  double bscalor = 1.;
  if (vm.count("bscalor")) {
    bscalor = vm["bscalor"].template as<double>();
    std::cout << "- BField scalor to Tesla set to: " << bscalor << std::endl;
  }
  std::shared_ptr<Acts::ConstantBField> cField 
    = std::make_shared<Acts::ConstantBField>(0.,0.,bscalor* Acts::units::_T);

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
  return bField ? 
    ACTFWExtrapolationExample::run(nEvents, bField, gtGeometry, logLevel):
    ACTFWExtrapolationExample::run(nEvents, cField, gtGeometry, logLevel);
}
