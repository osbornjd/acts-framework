//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
#include <cstdlib>
#include <memory>
#include <boost/program_options.hpp>
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "HelloWorldAlgorithm.hpp"

namespace po = boost::program_options;

// the main hello world executable
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc,10,2);
  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if needed
  // output messages
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options options
  auto standardOptions 
    = FW::Options::readStandardOptions<po::variables_map>(vm);

  // and now the hello world algorithm
  std::shared_ptr<FW::IAlgorithm> 
    hWorld(new FWE::HelloWorldAlgorithm(standardOptions.second));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.appendEventAlgorithms({hWorld});
  sequencer.run(standardOptions.first);
}
