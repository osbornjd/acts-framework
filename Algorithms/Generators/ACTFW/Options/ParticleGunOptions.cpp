// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Options/ParticleGunOptions.hpp"

#include <Acts/Utilities/Units.hpp>

#include "ACTFW/Generators/MultiplicityGenerators.hpp"
#include "ACTFW/Generators/ParametricProcessGenerator.hpp"
#include "ACTFW/Generators/VertexGenerators.hpp"
#include "ACTFW/Utilities/Options.hpp"

void
FW::Options::addParticleGunOptions(
    boost::program_options::options_description& opt)
{
  using namespace boost::program_options;

  opt.add_options()("pg-nparticles",
                    value<size_t>()->default_value(1.),
                    "number of particles.")(
      "pg-pdg",
      value<int>()->default_value(13),
      "PDG number of the particle, will be adjusted for charge flip.")(
      "pg-mass",
      value<double>()->default_value(105.),
      "mass of the particle in [MeV]")("pg-charge",
                                       value<double>()->default_value(-1.),
                                       "charge of the particle in [e]")(
      "pg-chargeflip",
      bool_switch(),
      "flip the charge (and change PDG accordingly).")(
      "pg-d0-range",
      value<read_range>()->multitoken()->default_value({0., 0.}),
      "range in which the d0 parameter is simulated in [mm]. Please hand"
      "over by simply seperating the values by space")(
      "pg-z0-range",
      value<read_range>()->multitoken()->default_value({0., 0.}),
      "range in which the z0 parameter is simulated in [mm]. Please hand"
      "over by simply seperating the values by space")(
      "pg-t0-range",
      value<read_range>()->multitoken()->default_value({0., 0.}),
      "range in which the t0 parameter is simulated in [ns]. Please hand"
      "over by simply seperating the values by space")(
      "pg-phi-range",
      value<read_range>()->multitoken()->default_value({-M_PI, M_PI}),
      "range in which the phi0 parameter is simulated. Please hand over by "
      "simply seperating the values by space")(
      "pg-eta-range",
      value<read_range>()->multitoken()->default_value({-4., 4.}),
      "range in which the eta parameter is simulated. Please hand over by "
      "simply seperating the values by space")(
      "pg-pt-range",
      value<read_range>()->multitoken()->default_value({0.1, 1e3}),
      "range in which the pt in [GeV] parameter is simulated. Please hand "
      "over by simply seperating the values by space");
}

FW::EventGenerator::Config
FW::Options::readParticleGunOptions(
    const boost::program_options::variables_map& vm)
{
  using namespace Acts::UnitLiterals;

  // read the range as vector (missing istream for std::array)
  auto d0  = vm["pg-d0-range"].template as<read_range>();
  auto z0  = vm["pg-z0-range"].template as<read_range>();
  auto t0  = vm["pg-t0-range"].template as<read_range>();
  auto phi = vm["pg-phi-range"].template as<read_range>();
  auto eta = vm["pg-eta-range"].template as<read_range>();
  auto pt  = vm["pg-pt-range"].template as<read_range>();

  ParametricProcessGenerator::Config pgCfg;
  pgCfg.numParticles = vm["pg-nparticles"].template as<size_t>();
  pgCfg.d0Range      = {{d0[0] * 1_mm, d0[1] * 1_mm}};
  pgCfg.z0Range      = {{z0[0] * 1_mm, z0[1] * 1_mm}};
  pgCfg.t0Range      = {{t0[0] * 1_ns, t0[1] * 1_ns}};
  pgCfg.phiRange     = {{phi[0], phi[1]}};
  pgCfg.etaRange     = {{eta[0], eta[1]}};
  pgCfg.ptRange      = {{pt[0] * 1_GeV, pt[1] * 1_GeV}};
  pgCfg.mass         = vm["pg-mass"].template as<double>() * 1_MeV;
  pgCfg.charge       = vm["pg-charge"].template as<double>() * 1_e;
  pgCfg.randomCharge = vm["pg-chargeflip"].template as<bool>();
  pgCfg.pdg          = vm["pg-pdg"].template as<int>();

  EventGenerator::Config cfg;
  cfg.generators = {
      {FixedMultiplicityGenerator{1},
       FixedVertexGenerator{{0.0, 0.0, 0.0, 0.0}},
       ParametricProcessGenerator{pgCfg}},
  };

  return cfg;
}
