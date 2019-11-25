// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

//#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Fatras/Kernel/SelectorList.hpp"
#include "Fatras/Selectors/ChargeSelectors.hpp"
#include "Fatras/Selectors/KinematicCasts.hpp"
#include "Fatras/Selectors/SelectorHelpers.hpp"
#include "FatrasAlgorithm.hpp"

#include <iostream>
#include "ACTFW/Utilities/OptionsFwd.hpp"

namespace FW {

namespace Options {

  /// @brief read the Fatras options
  ///
  /// Adding Fatras specific options to the Options package
  ///
  /// @param [in] opt_t The options object where the specific digitization
  /// options are attached to
  void
  addFatrasOptions(boost::program_options::options_description& opt);

  /// @brief read the fatras specific options and return a Config file
  ///
  ///@tparam omap_t Type of the options map
  ///@param vm the options map to be read out
  template <typename AMAP, typename simulator_t, typename event_collection_t>
  typename FatrasAlgorithm<simulator_t, event_collection_t>::Config
  readFatrasConfig(const AMAP& vm, simulator_t& simulator)
  {
    using namespace Acts::UnitLiterals;
    // Create a config
    typename FatrasAlgorithm<simulator_t, event_collection_t>::Config
        fatrasConfig(std::move(simulator));

    // set the collections
    fatrasConfig.simulatedHitCollection
        = vm["fatras-sim-hits"].template as<std::string>();
    fatrasConfig.simulatedEventCollection
        = vm["fatras-sim-particles"].template as<std::string>();

    typedef Fatras::ChargedSelector            CSelector;
    typedef Fatras::Max<Fatras::casts::absEta> CMaxEtaAbs;
    typedef Fatras::Min<Fatras::casts::pT>     CMinPt;
    typedef Fatras::SelectorListAND<CSelector, CMinPt, CMaxEtaAbs>
        ChargedSelector;

    typedef Fatras::NeutralSelector            NSelector;
    typedef Fatras::Max<Fatras::casts::absEta> NMaxEtaAbs;
    typedef Fatras::Min<Fatras::casts::E>      NMinE;

    simulator.chargedSelector.template get<CMaxEtaAbs>().valMax = 5.;
    simulator.chargedSelector.template get<CMinPt>().valMin     = 100._MeV;

    simulator.neutralSelector.template get<NMaxEtaAbs>().valMax = 5.;
    simulator.neutralSelector.template get<NMinE>().valMin      = 100._MeV;

    // and return the config
    return fatrasConfig;
  }

}  // namespace Options
}  // namespace FW
