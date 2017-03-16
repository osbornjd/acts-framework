#include "ACTFW/Examples/DD4hepExample/DD4hepDetector.hpp"
#include <memory>
#include <stdio.h>
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Examples/DD4hepExample/DD4hepDetector.hpp"
#include "ACTFW/Extrapolation/ExtrapolationTestAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Root/RootExCellWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Tools/CylinderVolumeBuilder.hpp"
#include "ACTS/Tools/CylinderVolumeHelper.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Tools/TrackingVolumeArrayCreator.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "DD4hep/LCDD.h"

namespace DD4hepExample {

std::shared_ptr<const Acts::TrackingGeometry>
trackingGeometry(std::string          xmlFileName,
                 Acts::Logging::Level lvl,
                 Acts::BinningType    bTypePhi,
                 Acts::BinningType    bTypeR,
                 Acts::BinningType    bTypeZ,
                 double               envelopeR,
                 double               envelopeZ)
{
  // create the tracking geometry as a shared pointer
  // for testing DD4hep
  // set up the geometry service
  DD4hepPlugin::GeometryService::Config gsConfig("DD4hepGeoSvc",
                                                 Acts::Logging::VERBOSE);
  gsConfig.xmlFileName = xmlFileName;
  gsConfig.lvl         = lvl;
  gsConfig.bTypePhi    = bTypePhi;
  gsConfig.bTypeR      = bTypeR;
  gsConfig.bTypeZ      = bTypeZ;
  gsConfig.envelopeR   = envelopeR;
  gsConfig.envelopeZ   = envelopeZ;
  auto geometrySvc = std::make_shared<DD4hepPlugin::GeometryService>(gsConfig);

  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry();
  // set the tracking geometry
  if (tGeometry)
    std::cout << "Successfully returned TrackingGeometry" << std::endl;
  return (tGeometry);
}

void
extrapolation(std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
              Acts::Logging::Level                          eLogLevel,
              size_t                                        nEvents,
              size_t                                        nTests,
              std::array<double, 2> etaRange,
              std::array<double, 2> phiRange)
{
  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
      new Acts::ConstantBField{{0., 0., 0.002}});  // field is given in kT

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FWE::initExtrapolator(tGeometry, magField, eLogLevel);

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.gauss_parameters   = {{0., 1.}};
  brConfig.uniform_parameters = {{0., 1.}};
  brConfig.landau_parameters  = {{1., 7.}};
  brConfig.gamma_parameters   = {{1., 1.}};
  std::shared_ptr<FW::RandomNumbersSvc> randomNumbers(
      new FW::RandomNumbersSvc(brConfig));

  // Write ROOT TTree
  FWRoot::RootExCellWriter::Config recWriterConfig("RootExCellWriter",
                                                   eLogLevel);
  recWriterConfig.fileName       = "$PWD/ExtrapolationTestion.root";
  recWriterConfig.treeName       = "ExtrapolationTestion";
  recWriterConfig.writeBoundary  = false;
  recWriterConfig.writeMaterial  = false;
  recWriterConfig.writeSensitive = true;
  recWriterConfig.writePassive   = false;
  std::shared_ptr<FW::IExtrapolationCellWriter> rootEcWriter(
      new FWRoot::RootExCellWriter(recWriterConfig));

  // the Algorithm with its configurations
  FWE::ExtrapolationTestAlgorithm::Config eTestConfig;
  eTestConfig.testsPerEvent           = nTests;
  eTestConfig.parameterType           = 0;
  eTestConfig.searchMode              = 0;
  eTestConfig.extrapolationEngine     = extrapolationEngine;
  eTestConfig.extrapolationCellWriter = rootEcWriter;
  eTestConfig.randomNumbers           = randomNumbers;
  eTestConfig.d0Defs                  = {{0., 0.}};
  eTestConfig.z0Defs                  = {{0., 0.}};
  eTestConfig.phiRange                = phiRange;
  eTestConfig.etaRange                = etaRange;
  eTestConfig.ptRange                 = {{1000., 100000.}};
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
}  // end of namespace DD4hepExample
