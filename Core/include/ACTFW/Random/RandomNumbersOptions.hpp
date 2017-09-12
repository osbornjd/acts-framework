///////////////////////////////////////////////////////////////////
// RandomNumbersOptions.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_RANDOMNUMBEROPTIONS_HPP
#define ACTFW_OPTIONS_RANDOMNUMBEROPTIONS_HPP

#include <iostream>
#include "ACTFW/Random/RandomNumbersSvc.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an rnd prefix
  template <class AOPT>
  void
  addRandomNumbersOptions(AOPT& opt){
    opt.add_options()
      ("rnd-seed",
       po::value<int>()->default_value(1234567890),
       "Seed of the random number engine.");
  }
  
  /// read the particle gun options and return a Config file
  template <class AMAP> 
  FW::RandomNumbersSvc::Config 
  readRandomNumbersConfig(const AMAP& vm){
    
    FW::RandomNumbersSvc::Config randomConfig;
    randomConfig.seed 
      = vm["evg-seed"].template as<int>();
    // return the config
    return randomConfig;  
  }
    
}
}

#endif // ACTFW_OPTIONS_RANDOMNUMBEROPTIONS_HPP