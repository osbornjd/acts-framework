#ifndef ACTFW_BFIELD_BFIELDEXAMPLE_H
#define ACTFW_BFIELD_BFIELDEXAMPLE_H

#include <boost/program_options.hpp>
#include <string>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/BField/BFieldFromFile.hpp"
#include "ACTFW/Plugins/BField/RootInterpolatedBFieldWriter.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"

/// The main executable
///
/// Creates an InterpolatedBFieldMap from a txt or csv file and writes out the
/// grid points and values of the map into root format. The Field can then be
/// displayed using the root script printBField.cpp

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
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
  if (!bField) {
    std::cout << "Bfield could not be set up. Exiting." << std::endl;
    return -1;
  }
  // get the log level from input
  Acts::Logging::Level logLevel  = Acts::Logging::INFO;  
  if (vm.count("loglevel")) {
    logLevel =   Acts::Logging::Level(vm["loglevel"].as<size_t>());    
    std::cout << "- the output log level is set to " << logLevel << std::endl;
  } else {
    std::cout << "- default log level is " << logLevel << std::endl;
  }
  
  // Create the InterpolatedBFieldWriter
  FW::BField::RootInterpolatedBFieldWriter::Config writerConfig;
  if (vm["rz"].as<bool>())
    writerConfig.gridType = FW::BField::GridType::rz;
  else
    writerConfig.gridType = FW::BField::GridType::xyz;
  writerConfig.treeName   = "bField";
  writerConfig.fileName   = vm["out"].as<std::string>();

  writerConfig.bField = bField;
  auto bFieldWriter
      = std::make_shared<FW::BField::RootInterpolatedBFieldWriter>(
          writerConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({bFieldWriter});
  sequencer.run(1);
}

#endif  // ACTFW_BFIELD_BFIELDEXAMPLE_H
