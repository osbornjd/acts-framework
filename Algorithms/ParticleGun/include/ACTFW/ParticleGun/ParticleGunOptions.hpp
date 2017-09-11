///////////////////////////////////////////////////////////////////
// ReadEvgenOptions.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_PARTICLEGUNOPTIONS_HPP
#define ACTFW_OPTIONS_PARTICLEGUNOPTIONS_HPP

#include <cstdlib>
#include <iostream>
#include <utility>
#include "ACTS/Utilities/Units.hpp"

namespace po = boost::program_options;

using range = std::vector<double>;

namespace au = Acts::units;

namespace std
{
  std::ostream& operator<<(std::ostream &os, const range &vec) 
  {    
    for (auto item : vec) 
    { 
      os << item << " "; 
    } 
    return os; 
  }
} 

namespace FW {

namespace Options {

  // common bfield options
  template <class AOPT>
  void
  addParticleGunOptions(AOPT& opt){
    opt.add_options()(
        "nparticles",
        po::value<size_t>()->default_value(100.),
        "number of particles.")(
        "pdg",
        po::value<int>()->default_value(13),
        "PDG number of the particle, "
        "will be adjusted for charge flip.")(
        "mass",
        po::value<double>()->default_value(105.*au::_MeV),
        "mass of the particle in [MeV]")(
        "charge",
        po::value<double>()->default_value(-1.),
        "charge of the particle in [e]")(
        "chargeflip",
        po::value<bool>()->default_value(true),
        "flip the charge (and change PDG accordingly).")(
        "d0range",
        po::value<range>()->default_value({0.,0.}),
        "range in which the d0 parameter is simulated.")(
        "z0range",
        po::value<range>()->default_value({0.,0.}),
        "range in which the z0 parameter is simulated.")(
        "phirange",
        po::value<range>()->default_value({-M_PI,M_PI}),
        "range in which the phi0 parameter is simulated.")(
        "etarange",
        po::value<range>()->default_value({-4.,4.}),
        "range in which the eta parameter is simulated.")(
        "ptrange",
        po::value<range>()->default_value({100.*au::_MeV,
                                           1e5*au::_MeV}),
        "range in which the pt in [MeV] parameter is simulated.");
  }
}
}

#endif // ACTFW_OPTIONS_PARTICLEGUNOPTIONS_HPP
