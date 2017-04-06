///////////////////////////////////////////////////////////////////
// MaterialMappingTest.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialStepWriter.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecReader.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecWriter.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapping.hpp"
#include "CreateMaterialMap.hpp"
#include "FullMaterialTest.hpp"
#include "SurfaceMaterialTest.hpp"
#include "MaterialMappingAlgorithm.hpp"

int
main()
{
  size_t               nEvents   = 100000;
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  // set up the geometry service
  DD4hepPlugin::GeometryService::Config gsConfig("GeometryService",
                                                 Acts::Logging::VERBOSE);
  gsConfig.xmlFileName
      = "file:Examples/DD4hepExample/DD4hepDetector/compact/FCCTracker.xml";
  auto geometrySvc = std::make_shared<DD4hepPlugin::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry();

  DD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4", eLogLevel);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc     = std::make_shared<DD4hepG4::DD4hepToG4Svc>(dgConfig);

  // set up the writer
  FWRoot::RootMaterialTrackRecWriter::Config writerConfig("MaterialTrackWriter",
                                                          eLogLevel);
  writerConfig.fileName = "MaterialTrackRecCollections.root";
  writerConfig.treeName = "MaterialTrackRecCollections";
  auto materialTrackRecWriter
      = std::make_shared<FWRoot::RootMaterialTrackRecWriter>(writerConfig);

  /// set up the reader
  FWRoot::RootMaterialTrackRecReader::Config readerConfig("MaterialReader",
                                                          eLogLevel);
  readerConfig.fileName = "MaterialTrackRecCollections.root";
  readerConfig.treeName = "MaterialTrackRecCollections";
  auto materialTrackRecReader
      = std::make_shared<FWRoot::RootMaterialTrackRecReader>(readerConfig);

  // set up the algorithm writing out the material map
  FWE::CreateMaterialMap::Config cmmConfig;
  cmmConfig.materialTrackRecWriter = materialTrackRecWriter;
  //  cmmConfig.gdmlFile              =
  //  "Examples/MaterialMappingTest/Geant4Detector.gdml";
  cmmConfig.geant4Service  = dd4hepToG4Svc;
  cmmConfig.numberOfEvents = nEvents;
  auto createMapAlg        = std::make_shared<FWE::CreateMaterialMap>(
      cmmConfig, Acts::getDefaultLogger("CreateMaterialMap", eLogLevel));

  // EXTRAPOLATOR - set up the extrapolator
  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magFieldSvc(
      new Acts::ConstantBField{{0., 0., 0.002}});  // field is given in kT
  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FWE::initExtrapolator(tGeometry, magFieldSvc, eLogLevel);

  // create material mapping
  Acts::MaterialMapping::Config mapperConf;
  mapperConf.extrapolationEngine = extrapolationEngine;
  auto materialMapper            = std::make_shared<Acts::MaterialMapping>(
      mapperConf, Acts::getDefaultLogger("MaterialMapping", eLogLevel));

  // set up the algorithm reading in the material map and mapping the material
  // onto the tracking geometry
  FWE::MaterialMappingAlgorithm::Config mmConfig;
  mmConfig.materialTrackRecReader = materialTrackRecReader;
  mmConfig.materialMapper         = materialMapper;
  mmConfig.trackingGeometry       = tGeometry;
  mmConfig.maximumTrackRecords    = 1000;
  auto materialMappingAlg = std::make_shared<FWE::MaterialMappingAlgorithm>(
      mmConfig, Acts::getDefaultLogger("MaterialMappingAlgorithm", eLogLevel));

  // create the material writer
  FWRoot::RootMaterialWriter::Config matWriterConfig("MaterialWriter",
                                                     eLogLevel);
  matWriterConfig.fileName = "$PWD/LayerMaterialMaps.root";
  auto materialWriter
      = std::make_shared<FWRoot::RootMaterialWriter>(matWriterConfig);

  // create the material step writer
  FWRoot::RootMaterialStepWriter::Config stepWriterConfig("MaterialStepWriter",
                                                          eLogLevel);
  stepWriterConfig.fileName = "$PWD/G4LayerMaterialMaps.root";
  auto stepWriter
      = std::make_shared<FWRoot::RootMaterialStepWriter>(stepWriterConfig);

  // set up the algorithm reading in the material map and mapping the material
  // onto the tracking geometry
  FWE::SurfaceMaterialTest::Config lmConfig;
  lmConfig.materialMapper     = materialMapper;
  lmConfig.materialWriter     = materialWriter;
  lmConfig.materialStepWriter = stepWriter;
  auto layerMaterialAlg       = std::make_shared<FWE::SurfaceMaterialTest>(
      lmConfig, Acts::getDefaultLogger("SurfaceMaterialTest", eLogLevel));

  // Now testing the full material

  // set up the writer
  FWRoot::RootMaterialTrackRecWriter::Config mtrConfig("MaterialTrackWriter",
                                                       eLogLevel);
  mtrConfig.fileName = "$PWD/FullMaterialRecords.root";
  mtrConfig.treeName = "FullMaterialRecords";
  auto fullMaterialWriter
      = std::make_shared<FWRoot::RootMaterialTrackRecWriter>(mtrConfig);

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.gauss_parameters   = {{0., 1.}};
  brConfig.uniform_parameters = {{0., 1.}};
  brConfig.landau_parameters  = {{1., 7.}};
  brConfig.gamma_parameters   = {{1., 1.}};
  std::shared_ptr<FW::RandomNumbersSvc> randomNumbers(
      new FW::RandomNumbersSvc(brConfig));

  FWE::FullMaterialTest::Config fmConfig;
  //  fmConfig.extrapolationEngine
  //     = FWE::initExtrapolator(tGeometry, magFieldSvc, eLogLevel);
  fmConfig.extrapolationEngine = extrapolationEngine;

  fmConfig.materialTrackRecWriter = fullMaterialWriter;
  fmConfig.randomNumbers          = randomNumbers;
  auto fullMaterialTest           = std::make_shared<FWE::FullMaterialTest>(
      fmConfig, Acts::getDefaultLogger("FullMatTest", eLogLevel));

  /*
  
  // Geant4 job - these can be many Geant4 jobs, indeed
  //
  // create the config object for the sequencer
  FW::Sequencer::Config seqG4Config;
  // now create the sequencer
  FW::Sequencer g4Sequencer(seqG4Config);
  //  sequencer.addServices({geometrySvc});
  //    sequencer.addServices({dd4hepToG4Svc});
  g4Sequencer.addServices({materialTrackRecWriter});
  g4Sequencer.appendEventAlgorithms({createMapAlg});

  // initialize loop
  g4Sequencer.initializeEventLoop();
  // run the loop
  g4Sequencer.processEventLoop(1);
  // finalize loop
  g4Sequencer.finalizeEventLoop();

  */
  
  // Mapping job 
  //
  // create the config object for the sequencer
  FW::Sequencer::Config mapSeqConfig;
  // now create the sequencer
  FW::Sequencer mappingSequencer(mapSeqConfig);
  mappingSequencer.addServices({materialTrackRecReader});
  mappingSequencer.appendEventAlgorithms({materialMappingAlg});

  // initialize loop
  mappingSequencer.initializeEventLoop();
  // run the loop
  mappingSequencer.processEventLoop(1);
  // finalize loop
  mappingSequencer.finalizeEventLoop();

  /*
  // create the config object for the sequencer
  FW::Sequencer::Config seq3Config;
  // now create the sequencer
  FW::Sequencer sequencer3(seq3Config);
  sequencer3.addServices({materialWriter});
  sequencer3.addServices({stepWriter});
  sequencer3.appendEventAlgorithms({layerMaterialAlg});

  // initialize loop
  sequencer3.initializeEventLoop();
  // run the loop
  sequencer3.processEventLoop(1);
  // finalize loop
  sequencer3.finalizeEventLoop();

  // create the config object for the sequencer
  FW::Sequencer::Config seq4Config;
  // now create the sequencer
  FW::Sequencer sequencer4(seq4Config);
  sequencer4.addServices({fullMaterialWriter});
  sequencer4.appendEventAlgorithms({fullMaterialTest});

  // initialize loop
  sequencer4.initializeEventLoop();
  // run the loop
  sequencer4.processEventLoop(nEvents);
  // finalize loop
  sequencer4.finalizeEventLoop();
  */
}
