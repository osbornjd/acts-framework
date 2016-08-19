///////////////////////////////////////////////////////////////////
// ExtrapolationUtils.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/ExtrapolationTest/ExtrapolationUtils.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/MagneticField/IMagneticFieldSvc.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"


std::shared_ptr<Acts::IExtrapolationEngine>
FWE::initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo, std::shared_ptr<Acts::IMagneticFieldSvc> magFieldSvc, Acts::Logging::Level eLogLevel)
{
    // EXTRAPOLATOR - set up the extrapolator
    // (a) RungeKuttaPropagtator
    auto propConfig         = Acts::RungeKuttaEngine::Config();
    propConfig.fieldService = magFieldSvc;
    auto propEngine         = std::make_shared<Acts::RungeKuttaEngine>(propConfig);
    // (b) MaterialEffectsEngine
    auto matConfig      = Acts::MaterialEffectsEngine::Config();
    auto materialEngine = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
     // (c) StaticNavigationEngine
    auto navConfig                  = Acts::StaticNavigationEngine::Config();
    navConfig.propagationEngine     = propEngine;
    navConfig.materialEffectsEngine = materialEngine;
    navConfig.trackingGeometry      = geo;
    auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
    // (d) the StaticEngine
    auto statConfig                  = Acts::StaticEngine::Config();
    statConfig.propagationEngine     = propEngine;
    statConfig.navigationEngine      = navEngine;
    statConfig.materialEffectsEngine = materialEngine;
    auto statEngine                  = std::make_shared<Acts::StaticEngine>(statConfig);
    // (e) the material engine
    auto exEngineConfig                 = Acts::ExtrapolationEngine::Config();
    exEngineConfig.trackingGeometry     = geo;
    exEngineConfig.propagationEngine    = propEngine;
    exEngineConfig.navigationEngine     = navEngine;
    exEngineConfig.extrapolationEngines = {statEngine};
    auto exEngine = std::make_shared<Acts::ExtrapolationEngine>(exEngineConfig);
    exEngine->setLogger(
                        Acts::getDefaultLogger("ExtrapolationEngine", eLogLevel));
    
    return exEngine;
}