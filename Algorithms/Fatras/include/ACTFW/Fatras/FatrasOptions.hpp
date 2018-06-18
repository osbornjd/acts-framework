// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Utilities/Options.hpp"
#include "FatrasAlgorithm.hpp"
#include "Fatras/Kernel/SelectorList.hpp"
#include "Fatras/Selectors/ChargeSelectors.hpp"
#include "Fatras/Selectors/SelectorHelpers.hpp"
#include "Fatras/Selectors/KinematicCasts.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  // common evgen options, with an evg prefix
  template <typename AOPT>
  void
  addFatrasOptions(AOPT& opt)
  {
    opt.add_options()(
        "fatras-sim-event",
        po::value<std::string>()->default_value("FatrasEvent"),
        "The collection of simulated particles.")(
        "fatras-sim-hits",
        po::value<std::string>()->default_value("FatrasHits"),
        "The collection of simulated hits")(
        "fatras-em-ionisation",
        po::value<bool>()->default_value(true),
        "Switch on ionisiation loss of charged particles")(
        "fatras-em-radiation",
        po::value<bool>()->default_value(true),
        "Switch on radiation for charged particles")(
        "fatras-em-scattering",
        po::value<bool>()->default_value(true),
        "Switch on multiple scattering")(
        "fatras-em-conversions",
        po::value<bool>()->default_value(true),
        "Switch on gamma conversions")(
        "fatras-had-interaction",
        po::value<bool>()->default_value(true),
        "Switch on hadronic interaction");
  }

  /// read the evgen options and return a Config file
  template <typename AMAP, 
            typename simulator_t, 
            typename event_collection_t,
            typename hit_t>
  typename FatrasAlgorithm<simulator_t,
                           event_collection_t,
                           hit_t>::Config
  readFatrasConfig(const AMAP& vm, simulator_t& simulator)
  {
    // create a config
    typename FatrasAlgorithm<simulator_t,
                             event_collection_t,
                             hit_t>::Config fatrasConfig(
        std::move(simulator));
    // set the collections
    fatrasConfig.simulatedHitCollection
        = vm["fatras-sim-hits"].template as<std::string>();
    fatrasConfig.simulatedEventCollection
        = vm["fatras-sim-event"].template as<std::string>();
    
    typedef Fatras::ChargedSelector             CSelector;  
    typedef Fatras::Max<Fatras::casts::absEta>  CMaxEtaAbs;
    typedef Fatras::Min<Fatras::casts::pT>      CMinPt;  
    typedef Fatras::SelectorListAND<CSelector,CMinPt,CMaxEtaAbs> ChargedSelector;
  
    typedef Fatras::NeutralSelector             NSelector;  
    typedef Fatras::Max<Fatras::casts::absEta>  NMaxEtaAbs;
    typedef Fatras::Min<Fatras::casts::E>       NMinE;  
    
    simulator.chargedSelector.template get<CMaxEtaAbs>().valMax = 5.;
    simulator.chargedSelector.template get<CMinPt>().valMin     = 100.*Acts::units::_MeV;

    simulator.neutralSelector.template get<NMaxEtaAbs>().valMax = 5.;
    simulator.neutralSelector.template get<NMinE>().valMin     = 100.*Acts::units::_MeV;
    
    // and return the config
    return fatrasConfig;
  }
  
  
  
  
} // namespace Options
} // namespace FW
