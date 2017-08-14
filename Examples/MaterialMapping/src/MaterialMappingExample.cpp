///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/MaterialMapping/MaterialMapping.hpp"
#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackReader.hpp"
#include "ACTFW/Plugins/Root/RootIndexedMaterialWriter.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"

int
main()
{
  size_t               nEvents   = 1;
  
  // DETECTOR:
  // --------------------------------------------------------------------------------
  // DD4Hep detector definition
  //
  // set up the geometry service
  FWDD4hep::GeometryService::Config gsConfig("GeometryService",
                                                 Acts::Logging::INFO);
  gsConfig.xmlFileName
      = "file:Examples/DD4hepExample/DD4hepDetector/compact/FCCTracker.xml";
  auto geometrySvc = std::make_shared<FWDD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry();
  
  // DD4Hep to Geant4 conversion
  //
  FWDD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4", Acts::Logging::INFO);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc     = std::make_shared<FWDD4hepG4::DD4hepToG4Svc>(dgConfig);
  
  // --------------------------------------------------------------------------------
  // MaterialMapping Algorithm configruation:
  //
  // set up the writer for the surface material maps
  FWRoot::RootMaterialTrackReader::Config mtrReaderConfig("MaterialTrackReader",
                                                             Acts::Logging::DEBUG);
  mtrReaderConfig.fileList = { "GeantMaterialTracks0.root",
                               "GeantMaterialTracks1.root"};
//                               "GeantMaterialTracks2.root",
//                               "GeantMaterialTracks3.root",
//                               "GeantMaterialTracks4.root"};
  mtrReaderConfig.treeName = "GeantMaterialTracks";
  auto mtrReader
      = std::make_shared<FWRoot::RootMaterialTrackReader>(mtrReaderConfig);
  
  // EXTRAPOLATOR - set up the extrapolator
  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magFieldSvc(
      new Acts::ConstantBField{{0., 0., 0.002}});  // field is given in kT
  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FW::initExtrapolator(tGeometry, magFieldSvc, Acts::Logging::INFO);

  // create material mapping
  Acts::MaterialMapper::Config mapperConf;
  mapperConf.extrapolationEngine = extrapolationEngine;
  auto mtrMapper            = std::make_shared<Acts::MaterialMapper>(
    mapperConf, Acts::getDefaultLogger("MaterialMapper", Acts::Logging::DEBUG));

  // create the mapped material writer
  // set up the writer for
  FWRoot::RootMaterialTrackWriter::Config mtrWriterConfig("MappedMaterialTrackWriter",
                                                            Acts::Logging::INFO);
  mtrWriterConfig.fileName = "MappedMaterialTracks.root";
  mtrWriterConfig.treeName = "MappedMaterialTracks";
  auto mtrWriter
      = std::make_shared<FWRoot::RootMaterialTrackWriter>(mtrWriterConfig);
  
                                                         
  // create the material writer
  FWRoot::RootIndexedMaterialWriter::Config imatWriterConfig("MaterialWriter",
                                                     Acts::Logging::INFO);
                                                     
  imatWriterConfig.fileName = "$PWD/LayerMaterialMaps.root";
  auto imaterialWriter
    = std::make_shared<FWRoot::RootIndexedMaterialWriter>(imatWriterConfig);
  
  // set up the algorithm reading in the material map and mapping the material
  // onto the tracking geometry
  FWA::MaterialMapping::Config mmConfig;
  mmConfig.materialTrackReader    = mtrReader;
  mmConfig.materialTrackWriter    = mtrWriter;
  mmConfig.materialMapper         = mtrMapper;
  mmConfig.indexedMaterialWriter  = imaterialWriter;
  mmConfig.trackingGeometry       = tGeometry;
  mmConfig.maximumTrackRecords    = 10000;
  auto materialMappingAlg = std::make_shared<FWA::MaterialMapping>(
      mmConfig, Acts::Logging::INFO);
  
  // --------------------------------------------------------------------------------
  // Mapping job configruation
  //
  // create the config object for the sequencer
  FW::Sequencer::Config mapSeqConfig;
  // now create the sequencer
  FW::Sequencer mappingSequencer(mapSeqConfig);
  mappingSequencer.addServices({mtrReader, mtrWriter, imaterialWriter});
  mappingSequencer.appendEventAlgorithms({materialMappingAlg});
  mappingSequencer.run(nEvents);
}
