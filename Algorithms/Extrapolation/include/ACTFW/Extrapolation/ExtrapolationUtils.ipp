// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"


template <class MagneticField>
std::unique_ptr<Acts::IExtrapolationEngine>
FW::initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo, std::shared_ptr<MagneticField> magFieldSvc, Acts::Logging::Level eLogLevel)
{
    // EXTRAPOLATOR - set up the extrapolator
    
    // (a) RungeKuttaPropagtator
    using RKEngine = Acts::RungeKuttaEngine<MagneticField>;
    typename RKEngine::Config propConfig;
    propConfig.fieldService = magFieldSvc;
    auto propEngine         = std::make_shared<RKEngine>(propConfig);
    propEngine->setLogger(Acts::getDefaultLogger("RungeKuttaEngine", eLogLevel));
    // (b) MaterialEffectsEngine
    Acts::MaterialEffectsEngine::Config matConfig;
    auto materialEngine = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
    materialEngine->setLogger(Acts::getDefaultLogger("MaterialEffectsEngine", eLogLevel));
    // (c) StaticNavigationEngine
    Acts::StaticNavigationEngine::Config navConfig;
    navConfig.propagationEngine     = propEngine;
    navConfig.materialEffectsEngine = materialEngine;
    navConfig.trackingGeometry      = geo;
    auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
    navEngine->setLogger(Acts::getDefaultLogger("NavigationEngine", eLogLevel));
    // (d) the StaticEngine
    Acts::StaticEngine::Config statConfig;
    statConfig.propagationEngine     = propEngine;
    statConfig.navigationEngine      = navEngine;
    statConfig.materialEffectsEngine = materialEngine;
    auto statEngine                  = std::make_shared<Acts::StaticEngine>(statConfig);
    statEngine->setLogger(Acts::getDefaultLogger("StaticEngine", eLogLevel));
    // (e) the material engine
    Acts::ExtrapolationEngine::Config exEngineConfig;
    exEngineConfig.trackingGeometry     = geo;
    exEngineConfig.propagationEngine    = propEngine;
    exEngineConfig.navigationEngine     = navEngine;
    exEngineConfig.extrapolationEngines = {statEngine};
    auto exEngine = std::make_unique<Acts::ExtrapolationEngine>(exEngineConfig);
    exEngine->setLogger(Acts::getDefaultLogger("ExtrapolationEngine", eLogLevel));
    // 
    return std::move(exEngine);
}
