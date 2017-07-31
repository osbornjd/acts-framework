
#include "ACTFW/FatrasValidation/G4FatrasValidation.hpp"
#include <iostream>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Geant4/FVRunAction.hpp"
#include "ACTFW/Plugins/Geant4/FVUserActionInitialization.hpp"
#include "ACTFW/Plugins/Geant4/PhysicsList.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "FTFP_BERT.hh"
#include "TGeoManager.h"

FWG4::G4FatrasValidation::G4FatrasValidation(
    const FWG4::G4FatrasValidation::Config& cfg,
    std::unique_ptr<const Acts::Logger>     logger)
  : FW::Algorithm(cfg, std::move(logger))
  , m_cfg(cfg)
  , m_runManager(new G4MTRunManager)
{
}

FWG4::G4FatrasValidation::~G4FatrasValidation()
{
  delete m_runManager;
}

FW::ProcessCode
FWG4::G4FatrasValidation::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // random number generator
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::ABORT;
  }
  // receive the geant4 geometry
  if (!m_cfg.geant4Service) {
    ACTS_FATAL("No Geant4 Service handed over!");
    return FW::ProcessCode::ABORT;
  }

  // receive the geant4 geometry
  if (!m_cfg.materialEffectsWriter) {
    ACTS_FATAL("No particle properties writer handed over!");
  }

  // initialize the runManager
  m_runManager->SetUserInitialization(m_cfg.geant4Service->geant4Geometry());
  m_runManager->SetUserInitialization(new FWG4::PhysicsList());
  m_runManager->SetUserInitialization(new FWG4::FVUserActionInitialization(
      m_cfg.pgaConfig, m_cfg.radialStepLimit));
  m_runManager->SetNumberOfThreads(1);
  m_runManager->Initialize();

  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWG4::G4FatrasValidation::execute(const FW::AlgorithmContext context) const
{
  m_runManager->BeamOn(m_cfg.testsPerEvent);

  auto particleProperties
      = FWG4::FVRunAction::Instance()->firstAndLastProperties();

  if (m_cfg.materialEffectsWriter) {
    for (auto& pProperties : particleProperties)
      m_cfg.materialEffectsWriter->write(pProperties);
  }

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWG4::G4FatrasValidation::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}
