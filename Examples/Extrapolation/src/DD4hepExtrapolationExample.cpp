#include <boost/program_options.hpp>
#include "ExtrapolationExampleBase.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"

namespace po = boost::program_options;

/// The main executable
///
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()(
      "input",
      po::value<std::string>()->default_value(
        "file:Detectors/DD4hepDetector/compact/FCChhTrackerTkLayout.xml"),
      "The location of the input DD4hep file, use 'file:foo.xml'");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,1,2);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);          
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // read the standard options
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options options
  auto standardOptions 
    = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents = standardOptions.first;
  auto logLevel = standardOptions.second;
  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);
  
  // particle gun as generator
  FW::ParticleGun::Config particleGunConfig;
  particleGunConfig.evgenCollection = "EvgenParticles";
  particleGunConfig.nParticles    = vm["dparticles"].as<size_t>();
  particleGunConfig.d0Range       = vm["d0range"].as<range>();
  particleGunConfig.z0Range       = vm["z0range"].as<range>();
  particleGunConfig.phiRange      = vm["phirange"].as<range>();
  particleGunConfig.etaRange      = vm["etarange"].as<range>();
  particleGunConfig.ptRange       = vm["ptrange"].as<range>();
  particleGunConfig.mass          = vm["mass"].as<double>() * Acts::units::_MeV;
  particleGunConfig.charge        = vm["charge"].as<double>() * Acts::units::_e;
  particleGunConfig.randomCharge  = vm["chargeflip"].as<bool>();
  particleGunConfig.pID           = vm["pdg"].as<int>() * Acts::units::_MeV;

  // get the DD4hep detector
  // DETECTOR:
  // --------------------------------------------------------------------------------
  FW::DD4hep::GeometryService::Config gsConfig("GeometryService",
                                              logLevel);
  gsConfig.xmlFileName              = vm["input"].as<std::string>();
  gsConfig.bTypePhi                 = Acts::equidistant;
  gsConfig.bTypeR                   = Acts::equidistant;
  gsConfig.bTypeZ                   = Acts::equidistant;
  gsConfig.envelopeR                = 0.;
  gsConfig.envelopeZ                = 0.;
  //gsConfig.buildDigitizationModules = false;

  auto geometrySvc = std::make_shared<FW::DD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
      = geometrySvc->trackingGeometry();

  // run the example - if you have a map run this, otherwise constant field
  return bField.first ? 
    ACTFWExtrapolationExample::run(nEvents, 
                                   bField.first, 
                                   dd4tGeometry, 
                                   particleGunConfig, 
                                   logLevel):
    ACTFWExtrapolationExample::run(nEvents, 
                                   bField.second, 
                                   dd4tGeometry, 
                                   particleGunConfig, 
                                   logLevel);
}
