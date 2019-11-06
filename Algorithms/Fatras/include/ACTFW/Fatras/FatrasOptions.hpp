// This file is part of the Acts project.
//
// Copyright (C) 2018-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <utility>

#include <Acts/Utilities/Units.hpp>
#include <boost/program_options.hpp>

#include "ACTFW/Fatras/FatrasAlgorithm.hpp"
#include "ACTFW/Utilities/OptionsFwd.hpp"
#include "Fatras/Kernel/SelectorList.hpp"
#include "Fatras/Selectors/ChargeSelectors.hpp"
#include "Fatras/Selectors/KinematicCasts.hpp"
#include "Fatras/Selectors/SelectorHelpers.hpp"

namespace FW {
namespace Options {

  /// Add Fatras options.
  ///
  /// @param desc The options description to add options to
  void
  addFatrasOptions(Description& desc);

  /// Read Fatras options to create the algorithm config.
  ///
  /// @tparam simulator_t type of the simulation kernel
  /// @param vars         the variables to read from
  /// @param simulator    the simulation kernel
  template <typename simulator_t>
  typename FatrasAlgorithm<simulator_t>::Config
  readFatrasConfig(const Variables& vars, simulator_t&& simulator)
  {
    using namespace Acts::UnitLiterals;
    using Config    = typename FatrasAlgorithm<simulator_t>::Config;
    using PtMin     = Fatras::Min<Fatras::casts::pT>;
    using EMin      = Fatras::Min<Fatras::casts::E>;
    using AbsEtaMax = Fatras::Max<Fatras::casts::absEta>;

    Config cfg(std::forward<simulator_t>(simulator));

    // set particle cuts
    cfg.simulator.chargedSelector.template get<PtMin>().valMin     = 100_MeV;
    cfg.simulator.chargedSelector.template get<AbsEtaMax>().valMax = 5.;
    cfg.simulator.neutralSelector.template get<EMin>().valMin      = 100_MeV;
    cfg.simulator.neutralSelector.template get<AbsEtaMax>().valMax = 5.;

    // set the input collections
    cfg.simulatedHitCollection
        = vars["fatras-sim-hits"].template as<std::string>();
    cfg.simulatedEventCollection
        = vars["fatras-sim-particles"].template as<std::string>();

    return cfg;
  }

}  // namespace Options
}  // namespace FW
