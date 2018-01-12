// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PYTHIA8OPTIONS_H
#define ACTFW_PYTHIA8OPTIONS_H

#include <iostream>
#include "ACTFW/Plugins/Pythia8/Generator.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common pythia options, with an evg prefix
  template <class AOPT>
  void
  addPythia8Options(AOPT& opt)
  {
    opt.add_options()("evg-cmsEnergy",
                      po::value<double>()->default_value(14000.),
                      "CMS value of the beam in [GeV].")(
        "evg-pdgBeam0",
        po::value<int>()->default_value(2212.),
        "PDG number of beam 0 particles.")(
        "evg-pdgBeam1",
        po::value<int>()->default_value(2212.),
        "PDG number of beam 1 particles.")(
        "evg-hsProcess",
        po::value<std::string>()->default_value("HardQCD:all = on"),
        "The process string for the hard scatter event.")(
        "evg-puProcess",
        po::value<std::string>()->default_value("SoftQCD:all = on"),
        "The process string for the pile-up events.")(
        "evg-hsSeed",
        po::value<int>()->default_value(123456789),
        "The generator seed for the hard scatter event.")(
        "evg-puSeed",
        po::value<int>()->default_value(234567890),
        "The generator seed for the pile-up events.");
  }

  /// read the particle gun options and return a Config file
  template <class AMAP>
  std::pair<FW::GPythia8::Generator::Config, FW::GPythia8::Generator::Config>
  readPythia8Config(const AMAP& vm)
  {
    // create a pythia generator for the hard scatter
    // process: HardQCD as default
    FW::GPythia8::Generator::Config hsPythiaConfig;
    hsPythiaConfig.pdgBeam0  = vm["evg-pdgBeam0"].template as<int>();
    hsPythiaConfig.pdgBeam1  = vm["evg-pdgBeam1"].template as<int>();
    hsPythiaConfig.cmsEnergy = vm["evg-cmsEnergy"].template as<double>();
    hsPythiaConfig.processStrings
        = {vm["evg-hsProcess"].template as<std::string>()};
    hsPythiaConfig.seed = {vm["evg-hsSeed"].template as<int>()};

    // create a pythia generator for the pile-up
    // MinBias with SD, DD and ND
    FW::GPythia8::Generator::Config puPythiaConfig;
    puPythiaConfig.pdgBeam0  = vm["evg-pdgBeam0"].template as<int>();
    puPythiaConfig.pdgBeam1  = vm["evg-pdgBeam1"].template as<int>();
    puPythiaConfig.cmsEnergy = vm["evg-cmsEnergy"].template as<double>();
    puPythiaConfig.processStrings
        = {vm["evg-puProcess"].template as<std::string>()};
    puPythiaConfig.seed = {vm["evg-puSeed"].template as<int>()};

    // return the pair of configs for the pythia generator
    return std::pair<FW::GPythia8::Generator::Config,
                     FW::GPythia8::Generator::Config>(hsPythiaConfig,
                                                      puPythiaConfig);
  }
}
}

#endif  // ACTFW_PYTHIA8OPTIONS_H