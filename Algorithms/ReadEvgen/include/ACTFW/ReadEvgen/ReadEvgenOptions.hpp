///////////////////////////////////////////////////////////////////
// ReadEvgenOptions.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_EVGENOPTIONS_HPP
#define ACTFW_OPTIONS_EVGENOPTIONS_HPP

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addEvgenOptions(AOPT& opt){
    opt.add_options()
      ("evg-collection",
       po::value<std::string>()->default_value("EvgenParticles"),
       "Collection name of the evgen particles.")
      ("evg-pileup",
       po::value<int>()->default_value(200),
       "Number of instantaneous pile-up events.")
      ("evg-vertex-xyrange",
       po::value<read_range>()->default_value({0., 0.015}),
       "transverse range of the vertex in xy.")
      ("evg-vertex-zrange",
       po::value<read_range>()->default_value({0., 5.5}),
       "transverse range of the vertex in z.");
        
  }
  
  /// read the particle gun options and return a Config file
  template <class AMAP> 
  FW::ReadEvgenAlgorithm::Config 
  readEvgenConfig(const AMAP& vm){
    
    FW::ReadEvgenAlgorithm::Config readEvgenConfig;
    readEvgenConfig.evgenCollection 
      = vm["evg-collection"].template as<std::string>();
    readEvgenConfig.pileupPoissonParameter 
      = vm["evg-pileup"].template as<int>();
    // vertex parameters
    auto vtpars = vm["evg-vertex-xyrange"].template as<read_range>();
    auto vzpars = vm["evg-vertex-xyrange"].template as<read_range>();
    readEvgenConfig.vertexTParameters  = {{vtpars[0], vtpars[1]}};
    readEvgenConfig.vertexZParameters  = {{vzpars[0], vzpars[1]}};
    // return the config
    return readEvgenConfig;  
  }
    
}
}

#endif // ACTFW_OPTIONS_EVGENOPTIONS_HPP