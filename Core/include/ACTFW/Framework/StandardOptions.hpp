///////////////////////////////////////////////////////////////////
// Options.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_FRAMEWORK_OPTIONS_HPP
#define ACTFW_FRAMEWORK_OPTIONS_HPP

#include <utility>
#include "ACTS/Utilities/Logger.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {



// add standard options
template < class AOPT>
void 
addStandardOptions(AOPT& opt, 
             size_t defaultEvents,
             size_t defaultValue){
  opt.add_options()("help", "Produce help message")
    ("events,n",
     po::value<size_t>()->default_value(defaultEvents),
     "The number of events to be processed")
    ("loglevel,l",
     po::value<size_t>()->default_value(defaultValue),
     "The output log level.");  
}

// read standard options
template <class AMAP> 
std::pair<size_t, Acts::Logging::Level>
readStandardOptions(const AMAP& vm){

  size_t nEvents = vm["events"].template as<size_t>();
  if (vm.count("events")){
    nEvents = vm["events"].template as<size_t>();
    std::cout << "- running " << nEvents << " events "<< std::endl;
  } else {
    std::cout << "- using standard number of events " << nEvents << std::endl;
  }
  Acts::Logging::Level logLevel
      = Acts::Logging::Level(vm["loglevel"].template as<size_t>());  
  if (vm.count("loglevel")) {
    logLevel =   Acts::Logging::Level(vm["loglevel"].template as<size_t>());    
    std::cout << "- the output log level is set to " << logLevel << std::endl;
  } else {
    std::cout << "- default log level is " << logLevel << std::endl;
  }
  return std::pair<size_t, Acts::Logging::Level>(nEvents,logLevel);  
}

}
}

#endif //
