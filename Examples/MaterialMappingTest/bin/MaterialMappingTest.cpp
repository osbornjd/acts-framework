///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecReader.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "../src/MaterialMapping.hpp"

int
main()
{
  // DETECTOR:
  // --------------------------------------------------------------------------------
  // DD4Hep detector definition
  //
  // set up the geometry service 
  DD4hepPlugin::GeometryService::Config gsConfig("GeometryService",
                                                 Acts::Logging::INFO);
  gsConfig.xmlFileName
      = "file:Examples/DD4hepExample/DD4hepDetector/compact/FCCTracker.xml";
  auto geometrySvc = std::make_shared<DD4hepPlugin::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry();
  
  // DD4Hep to Geant4 conversion
  //
  DD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4", Acts::Logging::INFO);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc     = std::make_shared<DD4hepG4::DD4hepToG4Svc>(dgConfig);
  
  // --------------------------------------------------------------------------------
  // MaterialMapping Algorithm configruation:
  // 
  // set up the writer for the surface material maps
  FWRoot::RootMaterialTrackRecReader::Config mtrReaderConfig("MaterialTrackReader",
                                                             Acts::Logging::INFO);
  mtrReaderConfig.fileName = "MaterialTrackRecCollections.root";
  mtrReaderConfig.treeName = "MaterialTrackRecCollections";
  auto mtrReader
      = std::make_shared<FWRoot::RootMaterialTrackRecReader>(mtrReaderConfig);
  
  // EXTRAPOLATOR - set up the extrapolator
  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magFieldSvc(
      new Acts::ConstantBField{{0., 0., 0.002}});  // field is given in kT
  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FWE::initExtrapolator(tGeometry, magFieldSvc, Acts::Logging::INFO);

  // create material mapping
  Acts::MaterialMapper::Config mapperConf;
  mapperConf.extrapolationEngine = extrapolationEngine;
  auto materialMapper            = std::make_shared<Acts::MaterialMapper>(
      mapperConf, Acts::getDefaultLogger("MaterialMapper", Acts::Logging::INFO));

  // create the material writer
  FWRoot::RootMaterialWriter::Config matWriterConfig("MaterialWriter",
                                                     Acts::Logging::INFO);
  matWriterConfig.fileName = "$PWD/LayerMaterialMaps.root";
  auto materialWriter
  = std::make_shared<FWRoot::RootMaterialWriter>(matWriterConfig);
  
  // set up the algorithm reading in the material map and mapping the material
  // onto the tracking geometry
  FWE::MaterialMapping::Config mmConfig;
  mmConfig.materialTrackRecReader = mtrReader;
  mmConfig.materialMapper         = materialMapper;
  mmConfig.materialWriter         = materialWriter;
  mmConfig.trackingGeometry       = tGeometry;
  mmConfig.maximumTrackRecords    = 1000;
  auto materialMappingAlg = std::make_shared<FWE::MaterialMapping>(
      mmConfig, Acts::getDefaultLogger("MaterialMapping", Acts::Logging::INFO));
  
  // --------------------------------------------------------------------------------
  // Mapping job configruation
  //
  // create the config object for the sequencer
  FW::Sequencer::Config mapSeqConfig;
  // now create the sequencer
  FW::Sequencer mappingSequencer(mapSeqConfig);
  mappingSequencer.addServices({mtrReader, materialWriter});
  mappingSequencer.appendEventAlgorithms({materialMappingAlg});

  // initialize loop
  mappingSequencer.initializeEventLoop();
  // run the loop
  mappingSequencer.processEventLoop(1);
  // finalize loop
  mappingSequencer.finalizeEventLoop();

}
