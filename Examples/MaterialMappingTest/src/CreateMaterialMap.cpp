///////////////////////////////////////////////////////////////////
// CreateMaterialMap.cpp
///////////////////////////////////////////////////////////////////

#include "CreateMaterialMap.hpp"
#include <iostream>
#include "ACTFW/Geant4MaterialMapping/MMDetectorConstruction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMEventAction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMPrimaryGeneratorAction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMRunAction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMSteppingAction.hpp"
#include "ACTFW/RootMaterialMapping/RootMaterialTrackRecWriter.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrackRecord.hpp"
#include "FTFP_BERT.hh"
#include "G4RunManager.hh"

FWE::CreateMaterialMap::CreateMaterialMap(
    const FWE::CreateMaterialMap::Config& cnf,
    std::unique_ptr<Acts::Logger>         log)
  : FW::Algorithm(cnf, std::move(log)), m_cnf(cnf)
{
}

FWE::CreateMaterialMap::~CreateMaterialMap()
{
}

FW::ProcessCode
FWE::CreateMaterialMap::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }

  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::CreateMaterialMap::execute(const FW::AlgorithmContext context) const
{
  G4RunManager* runManager = new G4RunManager;
  /// check if the geometry should be accessed over the geant4 service
  if (m_cnf.geant4Service) {
    runManager->SetUserInitialization(m_cnf.geant4Service->geant4Geometry());
  } else if (!m_cnf.gdmlFile.empty()) {
    /// access the geometry from the gdml file
    ACTS_INFO(
        "received Geant4 geometry from GDML file: " << m_cnf.gdmlFile.c_str());
    G4MM::MMDetectorConstruction* detConstruction
        = new G4MM::MMDetectorConstruction();
    detConstruction->setGdmlInput(m_cnf.gdmlFile.c_str());
    runManager->SetUserInitialization(
        detConstruction);  // constructs detector (calls Construct in
                           // Geant4DetectorConstruction)
  } else
    ACTS_FATAL("No geometry input for Geant4 given");
  /// bail out if not geometry input given

  /// Now set up the Geant4 simulation
  runManager->SetUserInitialization(new FTFP_BERT);
  runManager->SetUserAction(new G4MM::MMPrimaryGeneratorAction());
  G4MM::MMRunAction* runaction = new G4MM::MMRunAction();
  runManager->SetUserAction(runaction);
  runManager->SetUserAction(new G4MM::MMEventAction());
  runManager->SetUserAction(new G4MM::MMSteppingAction());
  runManager->Initialize();
  /*   G4GDMLParser parser;
   parser.Write("Geant4Detector.gdml",
   G4TransportationManager::GetTransportationManager()
   ->GetNavigatorForTracking()->GetWorldVolume()->GetLogicalVolume());
   */
  /// Begin with the simulation
  runManager->BeamOn(m_cnf.numberOfEvents);
  std::vector<Acts::MaterialTrackRecord> mtrecords
      = G4MM::MMEventAction::Instance()->materialTrackRecords();
  ACTS_INFO(
      "Received " << mtrecords.size()
                  << " MaterialTrackRecords. Writing them now onto file...");
  for (auto& record : mtrecords) {
    m_cnf.materialTrackRecWriter->write(record);
  }
  delete runManager;

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::CreateMaterialMap::finalize()
{
  ACTS_VERBOSE("finalize successful.");
  return FW::ProcessCode::SUCCESS;
}
