///////////////////////////////////////////////////////////////////
// ReadEvgenOptions.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_OPTIONS_PARTICLEGUNOPTIONS_HPP
#define ACTFW_OPTIONS_PARTICLEGUNOPTIONS_HPP

#include <cstdlib>
#include <iostream>
#include <utility>
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Utilities/Options.hpp"
#include "ACTS/Utilities/Units.hpp"

namespace po = boost::program_options;

namespace au = Acts::units;

namespace FW {

namespace Options {

  /// the particle gun options, the are prefixes with gp
  template <class AOPT>
  void
  addParticleGunOptions(AOPT& opt)
  {
    opt.add_options()("pg-nparticles",
                      po::value<size_t>()->default_value(100.),
                      "number of particles.")(
        "pg-pdg",
        po::value<int>()->default_value(13),
        "PDG number of the particle, will be adjusted for charge flip.")(
        "pg-mass",
        po::value<double>()->default_value(105.),
        "mass of the particle in [MeV]")(
        "pg-charge",
        po::value<double>()->default_value(-1.),
        "charge of the particle in [e]")(
        "pg-chargeflip",
        po::value<bool>()->default_value(true),
        "flip the charge (and change PDG accordingly).")(
        "pg-d0range",
        po::value<read_range>()->default_value({0., 0.}),
        "range in which the d0 parameter is simulated in [mm].")(
        "pg-z0range",
        po::value<read_range>()->default_value({0., 0.}),
        "range in which the z0 parameter is simulated in [mm].")(
        "pg-phirange",
        po::value<read_range>()->default_value({-M_PI, M_PI}),
        "range in which the phi0 parameter is simulated.")(
        "pg-etarange",
        po::value<read_range>()->default_value({-4., 4.}),
        "range in which the eta parameter is simulated.")(
        "pg-ptrange",
        po::value<read_range>()->default_value({100., 1e5}),
        "range in which the pt in [MeV] parameter is simulated.");
  }

  /// read the particle gun options and return a Config file
  template <class AMAP>
  FW::ParticleGun::Config
  readParticleGunConfig(const AMAP& vm)
  {
    // read the reange as vector (missing istream for std::array)
    auto d0r  = vm["pg-d0range"].template as<read_range>();
    auto z0r  = vm["pg-z0range"].template as<read_range>();
    auto phir = vm["pg-phirange"].template as<read_range>();
    auto etar = vm["pg-etarange"].template as<read_range>();
    auto ptr  = vm["pg-ptrange"].template as<read_range>();
    // particle gun as generator
    FW::ParticleGun::Config particleGunConfig;
    particleGunConfig.evgenCollection = "EvgenParticles";
    particleGunConfig.nParticles = vm["pg-nparticles"].template as<size_t>();
    particleGunConfig.d0Range    = {{d0r[0] * au::_mm, d0r[1] * au::_mm}};
    particleGunConfig.z0Range    = {{z0r[0] * au::_mm, z0r[1] * au::_mm}};
    particleGunConfig.phiRange   = {{phir[0], phir[1]}};
    particleGunConfig.etaRange   = {{etar[0], etar[1]}};
    particleGunConfig.ptRange    = {{ptr[0] * au::_MeV, ptr[1] * au::_MeV}};
    particleGunConfig.mass   = vm["pg-mass"].template as<double>() * au::_MeV;
    particleGunConfig.charge = vm["pg-charge"].template as<double>() * au::_e;
    particleGunConfig.randomCharge = vm["pg-chargeflip"].template as<bool>();
    particleGunConfig.pID          = vm["pg-pdg"].template as<int>();
    // return the config object
    return particleGunConfig;
  }
}
}

#endif  // ACTFW_OPTIONS_PARTICLEGUNOPTIONS_HPP
