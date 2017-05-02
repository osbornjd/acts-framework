///////////////////////////////////////////////////////////////////
// GenatinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecWriter.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "../src/GeantinoRecording.hpp"

int
main()
{
  // job steering: the number of events
  size_t               nEvents   = 100000;
  
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
  
  // Geant4 JOB:
  // --------------------------------------------------------------------------------
  // set up the writer for 
  FWRoot::RootMaterialTrackRecWriter::Config g4WriterConfig("MaterialTrackWriter",
                                                            Acts::Logging::INFO);
  g4WriterConfig.fileName = "MaterialTrackRecCollections.root";
  g4WriterConfig.treeName = "MaterialTrackRecCollections";
  auto g4TrackRecWriter
      = std::make_shared<FWRoot::RootMaterialTrackRecWriter>(g4WriterConfig);  
  
  // set up the algorithm writing out the material map
  FWE::GeantinoRecording::Config g4rConfig;
  g4rConfig.materialTrackRecWriter = g4TrackRecWriter;
  g4rConfig.geant4Service          = dd4hepToG4Svc;
  g4rConfig.numberOfEvents         = nEvents;
  // create the geant4 algorithm
  auto g4rAlgorithm    = std::make_shared<FWE::GeantinoRecording>(
      g4rConfig, Acts::getDefaultLogger("GeantinoRecording", Acts::Logging::INFO));
  
  // Geant4 job - these can be many Geant4 jobs, indeed
  //
  // create the config object for the sequencer
  FW::Sequencer::Config g4SeqConfig;
  // now create the sequencer
  FW::Sequencer g4Sequencer(g4SeqConfig);
  // the writer is a service as it needs initialize, finalize
  g4Sequencer.addServices({g4TrackRecWriter});
  g4Sequencer.appendEventAlgorithms({g4rAlgorithm});
  // initialize loop
  g4Sequencer.initializeEventLoop();
  // run the loop
  g4Sequencer.processEventLoop(1);
  // finalize loop
  g4Sequencer.finalizeEventLoop();
  // --------------------------------------------------------------------------------
  
}
