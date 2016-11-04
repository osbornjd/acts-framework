#include "ACTFW/ExtrapolationTest/ExtrapolationUtils.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"


template <class MagneticField = Acts::ConstantBField>
std::unique_ptr<Acts::IExtrapolationEngine>
FWE::initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo, std::shared_ptr<MagneticField> magFieldSvc, Acts::Logging::Level eLogLevel)
{
    // EXTRAPOLATOR - set up the extrapolator
    // (a) RungeKuttaPropagtator
    using RKEngine = Acts::RungeKuttaEngine<MagneticField>;
    typename RKEngine::Config propConfig{};
    propConfig.fieldService = magFieldSvc;
    auto propEngine         = std::make_shared<RKEngine>(propConfig);
    propEngine->setLogger(Acts::getDefaultLogger("RungeKuttaEngine", eLogLevel));
    // (b) MaterialEffectsEngine
    auto matConfig          = Acts::MaterialEffectsEngine::Config();
    auto materialEngine = std::make_shared<Acts::MaterialEffectsEngine>(matConfig);
    materialEngine->setLogger(Acts::getDefaultLogger("MaterialEffectsEngine", eLogLevel));
     // (c) StaticNavigationEngine
    auto navConfig                  = Acts::StaticNavigationEngine::Config();
    navConfig.propagationEngine     = propEngine;
    navConfig.materialEffectsEngine = materialEngine;
    navConfig.trackingGeometry      = geo;
    auto navEngine = std::make_shared<Acts::StaticNavigationEngine>(navConfig);
    navEngine->setLogger(Acts::getDefaultLogger("NavigationEngine", eLogLevel));
    // (d) the StaticEngine
    auto statConfig                  = Acts::StaticEngine::Config();
    statConfig.propagationEngine     = propEngine;
    statConfig.navigationEngine      = navEngine;
    statConfig.materialEffectsEngine = materialEngine;
    auto statEngine                  = std::make_shared<Acts::StaticEngine>(statConfig);
    statEngine->setLogger(Acts::getDefaultLogger("StaticEngine", eLogLevel));
    // (e) the material engine
    auto exEngineConfig                 = Acts::ExtrapolationEngine::Config();
    exEngineConfig.trackingGeometry     = geo;
    exEngineConfig.propagationEngine    = propEngine;
    exEngineConfig.navigationEngine     = navEngine;
    exEngineConfig.extrapolationEngines = {statEngine};
    auto exEngine = std::make_unique<Acts::ExtrapolationEngine>(exEngineConfig);
    exEngine->setLogger(Acts::getDefaultLogger("ExtrapolationEngine", eLogLevel));
    
    return std::move(exEngine);
}
