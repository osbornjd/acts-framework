// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Utilities/Options.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolation/ExtrapolationEngine.hpp"
#include "Acts/Extrapolation/IExtrapolationEngine.hpp"
#include "Acts/Extrapolation/MaterialEffectsEngine.hpp"
#include "Acts/Extrapolation/RungeKuttaEngine.hpp"
#include "Acts/Extrapolation/StaticEngine.hpp"
#include "Acts/Extrapolation/StaticNavigationEngine.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ExtrapolationAlgorithm.hpp"

namespace po = boost::program_options;

namespace FW {

namespace Options {

  /// @brief ExtrapolationAlgorithm options
  ///
  /// @tparam Type of the options class from boost
  template <typename AOPT>
  void
  addExtrapolationOptions(AOPT& opt)
  {
    opt.add_options()(
        "ext-charged-cells",
        po::value<std::string>()->default_value("exchell_charged"),
        "Name of the charged cell collection.")(
        "ext-neutral-cells",
        po::value<std::string>()->default_value("exchell_neutral"),
        "Name of the neutral cell collection.")(
        "ext-energyloss",
        po::value<bool>()->default_value(true),
        "Apply energy loss correction.")("ext-scattering",
                                         po::value<bool>()->default_value(true),
                                         "Apply scattering correction.")(
        "ext-nparticles",
        po::value<size_t>()->default_value(1000),
        "Number of particles to be extrapolated.")(
        "ext-d0-sigma",
        po::value<double>()->default_value(15. * Acts::units::_um),
        "Sigma of the transverse impact parameter.")(
        "ext-z0-sigma",
        po::value<double>()->default_value(55. * Acts::units::_mm),
        "Sigma of the longitudinal impact parameter.")(
        "ext-phi-range",
        po::value<read_range>()->multitoken()->default_value({-M_PI, M_PI}),
        "Azimutal angle phi range for extrapolated tracks.")(
        "ext-eta-range",
        po::value<read_range>()->multitoken()->default_value({-4., 4.}),
        "Pseudorapidity range for extrapolated tracks.")(
        "ext-pt-range",
        po::value<read_range>()->multitoken()->default_value(
            {100. * Acts::units::_MeV, 100. * Acts::units::_GeV}),
        "Transverse momentum range for extrapolated tracks [in GeV]");
  }

  /// read the evgen options and return a Config file
  ///
  /// @tparam vmap_t is the Type of the Parameter map to be read out
  /// @tparam bfield_t is the Type of the Magnetic field
  ///
  /// @param vm is the parameter map for the options
  /// @param magField is the magnetic field objects as shared pointer
  /// @param tGeometry is the tracking geometry object
  ///
  /// @returns a Config object for the ExtrapolationAlgorithm
  template <typename vmap_t, typename bfield_t>
  FW::ExtrapolationAlgorithm::Config
  readExtrapolationConfig(
      const vmap_t&                                 vm,
      std::shared_ptr<bfield_t>                     magField,
      std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
  {
    // Set up the Extrapolaiton engine - log level first
    auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

    // (a) RungeKuttaPropagtator
    using RKEngine = Acts::RungeKuttaEngine<bfield_t>;
    typename RKEngine::Config propConfig;
    propConfig.fieldService = magField;
    auto propEngine         = std::make_shared<RKEngine>(propConfig);
    propEngine->setLogger(Acts::getDefaultLogger("RungeKuttaEngine", logLevel));
    // (b) MaterialEffectsEngine
    Acts::MaterialEffectsEngine::Config matConfig;
    matConfig.eLossCorrection = vm["ext-energyloss"].template as<bool>();
    matConfig.mscCorrection   = vm["ext-scattering"].template as<bool>();

    auto materialEngine
        = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
    materialEngine->setLogger(
        Acts::getDefaultLogger("MaterialEffectsEngine", logLevel));
    // (c) StaticNavigationEngine
    Acts::StaticNavigationEngine::Config navConfig;
    navConfig.propagationEngine     = propEngine;
    navConfig.materialEffectsEngine = materialEngine;
    navConfig.trackingGeometry      = tGeometry;
    auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
    navEngine->setLogger(Acts::getDefaultLogger("NavigationEngine", logLevel));
    // (d) the StaticEngine
    Acts::StaticEngine::Config statConfig;
    statConfig.propagationEngine     = propEngine;
    statConfig.navigationEngine      = navEngine;
    statConfig.materialEffectsEngine = materialEngine;
    auto statEngine = std::make_shared<Acts::StaticEngine>(statConfig);
    statEngine->setLogger(Acts::getDefaultLogger("StaticEngine", logLevel));
    // (e) the material engine
    Acts::ExtrapolationEngine::Config exEngineConfig;
    exEngineConfig.trackingGeometry     = tGeometry;
    exEngineConfig.propagationEngine    = propEngine;
    exEngineConfig.navigationEngine     = navEngine;
    exEngineConfig.extrapolationEngines = {statEngine};
    auto exEngine = std::make_shared<Acts::ExtrapolationEngine>(exEngineConfig);
    exEngine->setLogger(
        Acts::getDefaultLogger("ExtrapolationEngine", logLevel));

    read_range iphir = vm["ext-phi-range"].template as<read_range>();
    read_range ietar = vm["ext-eta-range"].template as<read_range>();
    read_range iptr  = vm["ext-pt-range"].template as<read_range>();

    /// Create the config for the Extrapoaltion algorithm
    FW::ExtrapolationAlgorithm::Config exAlgConfig;
    exAlgConfig.extrapolationEngine = exEngine;
    exAlgConfig.nparticles = vm["ext-nparticles"].template as<size_t>();
    exAlgConfig.d0Sigma    = vm["ext-d0-sigma"].template as<double>();
    exAlgConfig.z0Sigma    = vm["ext-z0-sigma"].template as<double>();
    exAlgConfig.phiRange   = {iphir[0], iphir[1]};
    exAlgConfig.etaRange   = {ietar[0], ietar[1]};
    exAlgConfig.ptRange
        = {iptr[0] * Acts::units::_GeV, iptr[1] * Acts::units::_GeV};
    exAlgConfig.simulatedChargedExCellCollection
        = vm["ext-charged-cells"].template as<std::string>();
    exAlgConfig.simulatedNeutralExCellCollection
        = vm["ext-neutral-cells"].template as<std::string>();
    return exAlgConfig;
  }

}  // namespace Options
}  // namespace FW
