// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_OPTIONS_EXTRAPOLATIONOPTIONS_HPP
#define ACTFW_OPTIONS_EXTRAPOLATIONOPTIONS_HPP

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <class AOPT>
  void
  addExtrapolationOptions(AOPT& opt)
  {
    opt.add_options()("ext-fatras",
                      po::value<bool>()->default_value(true),
                      "Use the fatras option.")
                      ("ext-energyloss",
                      po::value<bool>()->default_value(true),
                      "Apply energy loss correction.")
                     ("ext-scattering",
                      po::value<bool>()->default_value(true),
                      "Apply scattering correction.")
                     ("ext-hadronic",
                      po::value<bool>()->default_value(true),
                      "Apply hadronic interaction (Fatras only).")
                     ("ext-min-pt",
                      po::value<double>()->default_value(100.*Acts::units::_MeV),
                      "Minimum transverse momentum.")
                     ("ext-max-d0",
                      po::value<double>()->default_value(1000.*Acts::units::_mm),
                      "Maximal transverse impact parameter (absolute).")
                     ("ext-max-z0",
                      po::value<double>()->default_value(3000.*Acts::units::_mm),
                      "Maximal longitudinal impact parameter (absolute).")
                     ("ext-max-eta",
                      po::value<double>()->default_value(5.),
                      "Maximal pseudo-rapidity (absolute).");
  }

  /// read the evgen options and return a Config file
  template <class AMAP>
  std::pair< std::array<bool, 4>, std::array<double, 4> >
  readExtrapolationOptions(const AMAP& vm)
  {
    // boolean steering
    bool fatras     = vm["ext-fatras"].template as<bool>();
    bool eloss      = vm["ext-energyloss"].template as<bool>();
    bool scattering = vm["ext-scattering"].template as<bool>();
    bool hadronic   = vm["ext-hadronic"].template as<bool>();
    // parameter
    double minPt   = vm["ext-min-pt"].template as<double>(); 
    double maxD0   = vm["ext-max-d0"].template as<double>();
    double maxZ0   = vm["ext-max-z0"].template as<double>();
    double maxEta  = vm["ext-max-eta"].template as<double>(); 
    // return the config
    std::array<bool, 4> steering = { fatras, eloss, scattering, hadronic };
    std::array<double, 4> parameters = { minPt, maxD0, maxZ0, maxEta };
    
    return std::pair< std::array<bool, 4>, std::array<double, 4> >( steering, parameters );

  }
}

}

#endif // ACTFW_OPTIONS_EXTRAPOLATIONOPTIONS_HPP
