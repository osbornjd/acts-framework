//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#include <stdio.h>
#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbers.hpp"
#include "ACTFW/Root/RootExCellWriter.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ExtrapolationTestAlgorithm.hpp"
#include "ConstantFieldSvc.hpp"


// the main hello world executable
int main (int argc, char *argv[]) {

    size_t nEvents = 500;
    
    // create the tracking geometry as a shared pointer
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry = Acts::trackingGeometry(Acts::Logging::INFO,0);
    
    // set up the magnetic field
    FWE::ConstantFieldSvc::Config cffConfig;
    cffConfig.name = "ConstantMagField";
    cffConfig.field = {{ 0., 0., 20000. }};
    std::shared_ptr<Acts::IMagneticFieldSvc> magFieldSvc(new FWE::ConstantFieldSvc(cffConfig));
    
    // EXTRAPOLATOR - set up the extrapolator
    // (a) RungeKuttaPropagtator
    Acts::RungeKuttaEngine::Config rungeKuttaConfig("RungeKuttaEngine");
    rungeKuttaConfig.fieldService = magFieldSvc;
    std::shared_ptr<Acts::IPropagationEngine> rungeKuttaEngine(new Acts::RungeKuttaEngine(rungeKuttaConfig));
    // (b) MaterialEffectsEngine
    Acts::MaterialEffectsEngine::Config materialEffectsConfig("MaterialEffectsEngine");
    std::shared_ptr<Acts::IMaterialEffectsEngine> materialEffects(new Acts::MaterialEffectsEngine(materialEffectsConfig));
    // (c) StaticNavigationEngine
    Acts::StaticNavigationEngine::Config staticNavigatorConfig("StaticNavigator", Acts::Logging::INFO);
    staticNavigatorConfig.propagationEngine = rungeKuttaEngine;
    staticNavigatorConfig.trackingGeometry  = tGeometry;
    std::shared_ptr<Acts::INavigationEngine> staticNavigator(new Acts::StaticNavigationEngine(staticNavigatorConfig));
    // (d) the StaticEngine
    Acts::StaticEngine::Config staticEngineConfig("StaticEngine", Acts::Logging::INFO);
    staticEngineConfig.propagationEngine     = rungeKuttaEngine;
    staticEngineConfig.materialEffectsEngine = materialEffects;
    staticEngineConfig.navigationEngine      = staticNavigator;
    std::shared_ptr<Acts::IExtrapolationEngine> staticEngine(new Acts::StaticEngine(staticEngineConfig));
    // (e) the material engine
    Acts::ExtrapolationEngine::Config extrapolationEngineConfig("ExtrapolationEngine", Acts::Logging::INFO);
    extrapolationEngineConfig.navigationEngine = staticNavigator;
    extrapolationEngineConfig.extrapolationEngines = { staticEngine };
    extrapolationEngineConfig.propagationEngine    = rungeKuttaEngine;
    extrapolationEngineConfig.trackingGeometry     = tGeometry;
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine(new Acts::ExtrapolationEngine(extrapolationEngineConfig));
    
    // RANDOM NUMBERS - Create the random number engine
    FW::RandomNumbers::Config brConfig("RandomNumbers");
    brConfig.gauss_parameters = {{ 0., 1. }};
    brConfig.uniform_parameters = {{ 0., 1. }};
    brConfig.landau_parameters = {{ 1., 7. }};
    brConfig.gamma_parameters = {{ 1., 1. }};
    std::shared_ptr<FW::RandomNumbers> randomNumbers(new FW::RandomNumbers(brConfig));
    
    // Write ROOT TTree
    FWRoot::RootExCellWriter::Config recWriterConfig("RootExCellWriter");
    recWriterConfig.fileName = "/Users/salzburg/Desktop/ExtrapolationTest.root";
    recWriterConfig.treeName = "ExtrapolationTest";
    std::shared_ptr<FW::IExtrapolationCellWriter> rootEcWriter(new FWRoot::RootExCellWriter(recWriterConfig));
    
    // the Algorithm with its configurations
    FWE::ExtrapolationTestAlgorithm::Config eTestConfig("ExtrapolationEngineTest");
    eTestConfig.testsPerEvent           = 100;
    eTestConfig.parameterType           = 0;
    eTestConfig.searchMode              = -1;
    eTestConfig.extrapolationEngine     = extrapolationEngine;
    eTestConfig.extrapolationCellWriter = rootEcWriter;
    eTestConfig.randomNumbers           = randomNumbers;
    eTestConfig.d0Defs                  = {{0.,2.}};
    eTestConfig.z0Defs                  = {{0.,50.}};
    eTestConfig.phiRange                = {{-M_PI,M_PI}};
    eTestConfig.etaRange                = {{-3.5,3.5}};
    eTestConfig.ptRange                 = {{10000.,100000.}};
    eTestConfig.particleType            = 3;
    eTestConfig.collectSensitive        = true;
    eTestConfig.collectPassive          = false;
    eTestConfig.collectBoundary         = false;
    eTestConfig.collectMaterial         = true;
    eTestConfig.sensitiveCurvilinear    = false;
    eTestConfig.pathLimit               = -1.;
    
    std::shared_ptr<FW::IAlgorithm> extrapolationAlg(new FWE::ExtrapolationTestAlgorithm(eTestConfig));
    
    // create the config object for the sequencer
    FW::Sequencer::Config seqConfig("Sequencer");
    // now create the sequencer
    FW::Sequencer sequencer(seqConfig);
    sequencer.addServices({rootEcWriter});
    sequencer.appendEventAlgorithms({extrapolationAlg});
    
    // initialize loop
    sequencer.initializeEventLoop();
    // run the loop
    sequencer.processEventLoop(nEvents);
    // finalize loop
    sequencer.finalizeEventLoop();
    
}
