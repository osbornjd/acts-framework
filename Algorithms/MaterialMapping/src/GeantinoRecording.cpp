///////////////////////////////////////////////////////////////////
// GeantinoRecording.cpp
///////////////////////////////////////////////////////////////////

#include <iostream>
#include "ACTFW/MaterialMapping/GeantinoRecording.hpp"
#include "ACTFW/Plugins/Geant4/MMDetectorConstruction.hpp"
#include "ACTFW/Plugins/Geant4/MMEventAction.hpp"
#include "ACTFW/Plugins/Geant4/MMPrimaryGeneratorAction.hpp"
#include "ACTFW/Plugins/Geant4/MMSteppingAction.hpp"
#include "ACTFW/Plugins/Geant4/MMRunAction.hpp"
#include "FTFP_BERT.hh"

FWA::GeantinoRecording::GeantinoRecording(
    const FWA::GeantinoRecording::Config& cnf,
    std::unique_ptr<Acts::Logger>         log)
  : FW::Algorithm(cnf, std::move(log))
  , m_cfg(cnf)
  , m_runManager(nullptr)
{
}

FWA::GeantinoRecording::~GeantinoRecording()
{
}

FW::ProcessCode
FWA::GeantinoRecording::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }
  
  m_runManager = new G4RunManager;
  /// check if the geometry should be accessed over the geant4 service
  if (m_cfg.geant4Service) {
    m_runManager->SetUserInitialization(m_cfg.geant4Service->geant4Geometry());
  } else if (!m_cfg.gdmlFile.empty()) {
    /// access the geometry from the gdml file
    ACTS_INFO(
        "received Geant4 geometry from GDML file: " << m_cfg.gdmlFile.c_str());
    FWG4::MMDetectorConstruction* detConstruction
        = new FWG4::MMDetectorConstruction();
    detConstruction->setGdmlInput(m_cfg.gdmlFile.c_str());
    m_runManager->SetUserInitialization(
        detConstruction);  // constructs detector (calls Construct in
                           // Geant4DetectorConstruction)
  } else {
    ACTS_FATAL("No geometry input for Geant4 given");
    return FW::ProcessCode::ABORT;
  }

  /// Now set up the Geant4 simulation
  m_runManager->SetUserInitialization(new FTFP_BERT);
  m_runManager->SetUserAction(
    new FWG4::MMPrimaryGeneratorAction( "geantino", 
                                        1000.,
                                        m_cfg.seed1, 
                                        m_cfg.seed2));
  FWG4::MMRunAction* runaction = new FWG4::MMRunAction();
  m_runManager->SetUserAction(runaction);
  m_runManager->SetUserAction(new FWG4::MMEventAction());
  m_runManager->SetUserAction(new FWG4::MMSteppingAction());
  m_runManager->Initialize();
  
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::GeantinoRecording::execute(const FW::AlgorithmContext) const
{
  
  /// Begin with the simulation 
  m_runManager->BeamOn(m_cfg.tracksPerEvent);
  ///   
  std::vector<Acts::MaterialTrack> mtrecords
      = FWG4::MMEventAction::Instance()->MaterialTracks();
  ACTS_INFO(
      "Received " << mtrecords.size()
                  << " MaterialTracks. Writing them now onto file...");
  // write to the file
  for (auto& record : mtrecords) {
    m_cfg.materialTrackWriter->write(record);
  }

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::GeantinoRecording::finalize()
{
  // delete the run manager for this
  delete m_runManager;
  // and finalize 
  ACTS_VERBOSE("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}
