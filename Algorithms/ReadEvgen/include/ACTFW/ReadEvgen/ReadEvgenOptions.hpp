///////////////////////////////////////////////////////////////////
// ReadEvgenOptions.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_EVGENOPTIONS_HPP
#define ACTFW_OPTIONS_EVGENOPTIONS_HPP

#include <iostream>

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common bfield options
  template <class AOPT>
  void
  addEvgenOptions(AOPT& opt){
    opt.add_options()(
        "cmsEnergy",
        po::value<double>()->default_value(14000.),
        "CMS value of the beam in [GeV].")(
        "pdgBeam0",
        po::value<int>()->default_value(2212.),
        "PDG number of beam 0 particles.")(
        "pdgBeam1",
        po::value<int>()->default_value(2212.),
        "PDG number of beam 1 particles.")(
        "hsProcress",
        po::value<std::string>()->default_value("HardQCD:all = on"),
        "The process string for the hard scatter event.")(
        "puProcress",
        po::value<std::string>()->default_value("SoftQCD:all = on"),
        "The process string for the pile-up events.")(
        "pileup",
        po::value<int>()->default_value(200),
        "Number of instantaneous pile-up events.");
        
  }
}
}

#endif // ACTFW_OPTIONS_BFIELDOPTIONS_HPP