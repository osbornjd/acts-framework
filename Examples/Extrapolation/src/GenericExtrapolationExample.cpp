#include <boost/program_options.hpp>
#include "ExtrapolationExampleBase.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTFW/GenericDetector/BuildGenericDetector.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/ParticleGun/ParticleGunOptions.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"

namespace po = boost::program_options;

// the main executable
int
main(int argc, char* argv[])
{  
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,100,2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc); 
  // add the particle gun options
  FW::Options::addParticleGunOptions<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);                
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions 
    = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents = standardOptions.first;
  auto logLevel = standardOptions.second;
  // read and create BField service
  auto bField 
    = FW::Options::readBField<po::variables_map>(vm);
  // read and create  ParticleGunConfig
  auto particleGunConfig 
    = FW::Options::readParticleGunConfig<po::variables_map>(vm);
  // read and create RandomNumbersConfig
  auto randomNumbersConfig 
    = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);

  // get the generic detector
  // DETECTOR:
  // --------------------------------------------------------------------------------
  // create the tracking geometry as a shared pointer
  std::shared_ptr<const Acts::TrackingGeometry> gtGeometry
      = FWGen::buildGenericDetector(logLevel, logLevel, logLevel, 3);

  // run the example
  return bField.first ? 
    ACTFWExtrapolationExample::run(nEvents, 
                                   bField.first, 
                                   gtGeometry, 
                                   particleGunConfig,
                                   randomNumbersConfig, 
                                   logLevel):
    ACTFWExtrapolationExample::run(nEvents, 
                                   bField.second, 
                                   gtGeometry, 
                                   particleGunConfig, 
                                   randomNumbersConfig, 
                                   logLevel);
}