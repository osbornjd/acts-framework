//
//  ExtrapolationExample.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 100;
  
  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel  = Acts::Logging::INFO;
  Acts::Logging::Level layerLogLevel    = Acts::Logging::INFO;
  Acts::Logging::Level volumeLogLevel   = Acts::Logging::INFO;
  
  // create the tracking geometry as a shared pointer
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry
    = Acts::buildGenericDetector(surfaceLogLevel,
                                 layerLogLevel,
                                 volumeLogLevel,
                                 3);

  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
       new Acts::ConstantBField{{0., 0., 0.002}});
  
  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
    = FWA::initExtrapolator(tGeometry,magField,eLogLevel);

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.gauss_parameters   = {{0., 1.}};
  brConfig.uniform_parameters = {{0., 1.}};
  brConfig.landau_parameters  = {{1., 7.}};
  brConfig.gamma_parameters   = {{1., 1.}};
  std::shared_ptr<FW::RandomNumbersSvc> randomNumbers(
      new FW::RandomNumbersSvc(brConfig));

  // Write ROOT TTree
  // ecc for charged particles
  FWRoot::RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.fileName            = "$PWD/ExtrapolationChargedTest.root";
  reccWriterConfig.treeName            = "ExtrapolationChargedTest";
  reccWriterConfig.writeBoundary       = false;
  reccWriterConfig.writeMaterial       = true;
  reccWriterConfig.writeSensitive      = true;
  reccWriterConfig.writePassive        = true;
  std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters> > >
  rootEccWriter(
         new FWRoot::RootExCellWriter<Acts::TrackParameters>(reccWriterConfig));

  
  // ecc for neutral particles
  FWRoot::RootExCellWriter<Acts::NeutralParameters>::Config recnWriterConfig;
  recnWriterConfig.fileName            = "$PWD/ExtrapolationNeutralTest.root";
  recnWriterConfig.treeName            = "ExtrapolationNeutralTest";
  recnWriterConfig.writeBoundary       = false;
  recnWriterConfig.writeMaterial       = true;
  recnWriterConfig.writeSensitive      = true;
  recnWriterConfig.writePassive        = true;
  std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::NeutralParameters> > >
  rootEcnWriter(
                new FWRoot::RootExCellWriter<Acts::NeutralParameters>(recnWriterConfig));
  
  
  // the Algorithm with its configurations
  FWA::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.testsPerEvent           = 250;
  eTestConfig.parameterType           = 0;
  eTestConfig.searchMode              = 1;
  eTestConfig.extrapolationEngine     = extrapolationEngine;
  eTestConfig.ecChargedWriter         = rootEccWriter;
  eTestConfig.ecNeutralWriter         = rootEcnWriter;
  eTestConfig.randomNumbers           = randomNumbers;
  eTestConfig.d0Defs                  = {{0., 0.}};
  eTestConfig.z0Defs                  = {{0., 0.}};
  eTestConfig.phiRange                = {{-M_PI, M_PI}};
  eTestConfig.etaRange                = {{ -3.75, 3.75}};
  eTestConfig.ptRange                 = {{ 100., 1000.}};
  eTestConfig.particleType            = 3;
  eTestConfig.collectSensitive        = true;
  eTestConfig.collectPassive          = true;
  eTestConfig.collectBoundary         = true;
  eTestConfig.collectMaterial         = true;
  eTestConfig.sensitiveCurvilinear    = false;
  eTestConfig.pathLimit               = -1.;

  std::shared_ptr<FW::IAlgorithm> extrapolationAlg(
      new FWA::ExtrapolationAlgorithm(eTestConfig,
      Acts::getDefaultLogger("ExtrapolationAlgorithm", eLogLevel)));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({rootEccWriter, rootEcnWriter, randomNumbers});
  sequencer.appendEventAlgorithms({extrapolationAlg});

  // initialize loop
  sequencer.initializeEventLoop();
  // run the loop
  sequencer.processEventLoop(nEvents);
  // finalize loop
  sequencer.finalizeEventLoop();
}
