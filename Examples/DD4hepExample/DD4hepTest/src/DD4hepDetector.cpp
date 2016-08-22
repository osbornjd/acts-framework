#include "ACTFW/Examples/DD4hepExample/DD4hepDetector.hpp"
#include <memory>
#include <stdio.h>
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Examples/DD4hepExample/DD4hepDetector.hpp"
#include "ACTFW/ExtrapolationTest/ExtrapolationTestAlgorithm.hpp"
#include "ACTFW/ExtrapolationTest/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbers.hpp"
#include "ACTFW/Root/RootExCellWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/MagneticField/ConstantFieldSvc.hpp"
#include "ACTS/Plugins/DD4hepPlugins/DD4hepCylinderGeometryBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "DD4hep/LCDD.h"

namespace DD4hepExample {

std::shared_ptr<const Acts::TrackingGeometry>
trackingGeometry(std::string xmlFileName, Acts::Logging::Level lvl)
{
  // create the tracking geometry as a shared pointer
  // for testing DD4hep
  // set up the geometry service
  DD4hepPlugin::GeometryService::Config gsConfig("DD4hepGeoSvc",
                                                 Acts::Logging::VERBOSE);
  gsConfig.xmlFileName = xmlFileName;
  gsConfig.lvl         = lvl;
  auto geometrySvc = std::make_shared<DD4hepPlugin::GeometryService>(gsConfig);

  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry();
  // set the tracking geometry
  return (tGeometry);
}

void
extrapolation(std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
              Acts::Logging::Level                          eLogLevel)
{
  size_t nEvents = 1000;

  // set up the magnetic field
  Acts::ConstantFieldSvc::Config cffConfig;
  cffConfig.name  = "ConstantMagField";
  cffConfig.field = {{0., 0., 0.002}};
  std::shared_ptr<Acts::IMagneticFieldSvc> magFieldSvc(
      new Acts::ConstantFieldSvc(cffConfig));

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FWE::initExtrapolator(tGeometry, magFieldSvc, eLogLevel);

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbers::Config brConfig;
  brConfig.gauss_parameters   = {{0., 1.}};
  brConfig.uniform_parameters = {{0., 1.}};
  brConfig.landau_parameters  = {{1., 7.}};
  brConfig.gamma_parameters   = {{1., 1.}};
  std::shared_ptr<FW::RandomNumbers> randomNumbers(
      new FW::RandomNumbers(brConfig));

  // Write ROOT TTree
  FWRoot::RootExCellWriter::Config recWriterConfig("RootExCellWriter",
                                                   eLogLevel);
  recWriterConfig.fileName = "$PWD/ExtrapolationTestion.root";
  recWriterConfig.treeName = "ExtrapolationTestion";
  std::shared_ptr<FW::IExtrapolationCellWriter> rootEcWriter(
      new FWRoot::RootExCellWriter(recWriterConfig));

  // the Algorithm with its configurations
  FWE::ExtrapolationTestAlgorithm::Config eTestConfig;
  eTestConfig.name                    = "ExtrapolationEngineTest";
  eTestConfig.testsPerEvent           = 100;
  eTestConfig.parameterType           = 0;
  eTestConfig.extrapolationEngine     = extrapolationEngine;
  eTestConfig.extrapolationCellWriter = rootEcWriter;
  eTestConfig.randomNumbers           = randomNumbers;
  eTestConfig.d0Defs                  = {{0., 2.}};
  eTestConfig.z0Defs                  = {{0., 50.}};
  eTestConfig.phiRange                = {{-M_PI, M_PI}};
  eTestConfig.etaRange                = {{-1., 1.}};
  eTestConfig.ptRange                 = {{10000., 100000.}};
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
}
