// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_OPTIONS_TRUTHTRACKINGOPTIONS_HPP
#define ACTFW_OPTIONS_TRUTHTRACKINGOPTIONS_HPP

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "TruthTracking.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <typename AOPT>
  void
  addTruthTrackingOptions(AOPT& opt)
  {
    opt.add_options()(
        "ttrack-min-pt",
        po::value<double>()->default_value(1000. * Acts::units::_MeV),
        "Minimum transverse momentum for tracks to be fitted.")(
        "ttrack-max-vr",
        po::value<double>()->default_value(1000. * Acts::units::_mm),
        "Maximal vertex radius for tracks to be fitted.")(
        "ttrack-max-vz",
        po::value<double>()->default_value(3000. * Acts::units::_mm),
        "Maximal longitudinal vertex position for tracks to be fitted.")(
        "ttrack-max-eta",
        po::value<double>()->default_value(5.),
        "Maximal (absolute) pseudo-rapidity for tracks to be fitted.")(
        "ttrack-min-hits",
        po::value<unsigned int>()->default_value(7),
        "Minimum number of hits on tracks to be fitted.")(
        "ttrack-input-clusters",
        po::value<std::string>()->default_value("FatrasClusters"),
        "The input collection of clusters.")(
        "ttrack-output-tracks",
        po::value<std::string>()->default_value("FatrasTracks"),
        "The output collection of fitted tracks");
  }

  /// read the evgen options and return a Config file
  template <typename AMAP, typename Propagator_type>
  typename TruthTracking<Propagator_type>::Config
  readTruthTrackingConfig(const AMAP& vm, Propagator_type propagator)
  {
    // create a config
    typename TruthTracking<Propagator_type>::Config ttrackConfig(
        std::move(propagator));
    // read configuration from map
    // parameter
    ttrackConfig.minPt   = vm["ttrack-min-pt"].template as<double>();
    ttrackConfig.maxVr   = vm["ttrack-max-vr"].template as<double>();
    ttrackConfig.maxVz   = vm["ttrack-max-vz"].template as<double>();
    ttrackConfig.maxEta  = vm["ttrack-max-eta"].template as<double>();
    ttrackConfig.minHits = vm["ttrack-min-hits"].template as<unsigned int>();
    // set the collections
    ttrackConfig.clusterCollection
        = vm["ttrack-input-clusters"].template as<std::string>();
    ttrackConfig.trackCollection
        = vm["ttrack-output-tracks"].template as<std::string>();
    // and return the config
    return ttrackConfig;
  }
}
}

#endif  // ACTFW_OPTIONS_FATRASOPTIONS_HPP
