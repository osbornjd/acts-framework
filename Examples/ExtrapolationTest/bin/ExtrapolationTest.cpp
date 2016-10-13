//
//  HelloWorld.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbers.hpp"
#include "ACTFW/Root/RootExCellWriter.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/MagneticField/ConstantFieldSvc.hpp"
#include "ACTFW/ExtrapolationTest/ExtrapolationTestAlgorithm.hpp"
#include "ACTFW/ExtrapolationTest/ExtrapolationUtils.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 100;
  
  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel = Acts::Logging::INFO;
  Acts::Logging::Level layerLogLevel = Acts::Logging::INFO;
  Acts::Logging::Level volumeLogLevel = Acts::Logging::INFO;
  
  // create the tracking geometry as a shared pointer
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry
    = Acts::buildGenericDetector(surfaceLogLevel,
                                 layerLogLevel,
                                 volumeLogLevel,
                                 3);

  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  // set up the magnetic field
  Acts::ConstantFieldSvc::Config cffConfig;
  cffConfig.name  = "ConstantMagField";
  cffConfig.field = {{0., 0., 0.002}};  // field is given in kT
  std::shared_ptr<Acts::IMagneticFieldSvc> magFieldSvc(
      new Acts::ConstantFieldSvc(cffConfig));

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = FWE::initExtrapolator(tGeometry,magFieldSvc,eLogLevel);

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbers::Config brConfig;
  brConfig.gauss_parameters   = {{0., 1.}};
  brConfig.uniform_parameters = {{0., 1.}};
  brConfig.landau_parameters  = {{1., 7.}};
  brConfig.gamma_parameters   = {{1., 1.}};
  std::shared_ptr<FW::RandomNumbers> randomNumbers(
      new FW::RandomNumbers(brConfig));

  // Write ROOT TTree
  FWRoot::RootExCellWriter::Config recWriterConfig;
  recWriterConfig.fileName            = "$PWD/ExtrapolationTest.root";
  recWriterConfig.treeName            = "ExtrapolationTest";
  recWriterConfig.writeBoundary       = false;
  recWriterConfig.writeMaterial       = false;
  recWriterConfig.writeSensitive      = true;
  recWriterConfig.writePassive        = false;
  std::shared_ptr<FW::IExtrapolationCellWriter> rootEcWriter(
      new FWRoot::RootExCellWriter(recWriterConfig));

  // the Algorithm with its configurations
  FWE::ExtrapolationTestAlgorithm::Config eTestConfig;
  eTestConfig.testsPerEvent           = 100;
  eTestConfig.parameterType           = 0;
  eTestConfig.searchMode              = 1;
  eTestConfig.extrapolationEngine     = extrapolationEngine;
  eTestConfig.extrapolationCellWriter = rootEcWriter;
  eTestConfig.randomNumbers           = randomNumbers;
  eTestConfig.d0Defs                  = {{0., 0.}};
  eTestConfig.z0Defs                  = {{0., 0.}};
  eTestConfig.phiRange                = {{-M_PI, M_PI}};
  eTestConfig.etaRange                = {{ -2.75, 2.75}};
  eTestConfig.ptRange                 = {{ 1000., 10000.}};
  eTestConfig.particleType            = 3;
  eTestConfig.collectSensitive        = true;
  eTestConfig.collectPassive          = true;
  eTestConfig.collectBoundary         = true;
  eTestConfig.collectMaterial         = true;
  eTestConfig.sensitiveCurvilinear    = false;
  eTestConfig.pathLimit               = -1.;

  std::shared_ptr<FW::IAlgorithm> extrapolationAlg(
      new FWE::ExtrapolationTestAlgorithm(eTestConfig));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
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
