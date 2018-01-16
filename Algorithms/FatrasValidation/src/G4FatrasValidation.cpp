// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

FW::G4::G4FatrasValidation::G4FatrasValidation(
    const FW::G4::G4FatrasValidation::Config& cfg,
    Acts::Logging::Level                      loglevel)
  : FW::BareAlgorithm("G4FatrasValidation", loglevel)
  , m_cfg(cfg)
  , m_runManager(new G4MTRunManager)
{
  // receive the geant4 geometry
  if (!m_cfg.geant4Service)
    throw std::invalid_argument("No Geant4 Service handed over!");

  // Validate the configuration
  if (m_cfg.particlePropertiesCollection.empty()) {
    throw std::invalid_argument("Missing input collection");
  }

  // initialize the runManager
  m_runManager->SetUserInitialization(m_cfg.geant4Service->geant4Geometry());
  m_runManager->SetUserInitialization(new FW::G4::PhysicsList());
  m_runManager->SetUserInitialization(new FW::G4::FVUserActionInitialization(
      m_cfg.pgaConfig, m_cfg.radialStepLimit));
  m_runManager->SetNumberOfThreads(1);
  m_runManager->Initialize();

  ACTS_VERBOSE("initialize successful.");
}

FW::G4::G4FatrasValidation::~G4FatrasValidation()
{
  delete m_runManager;
}

FW::ProcessCode
FW::G4::G4FatrasValidation::execute(const FW::AlgorithmContext context) const
{
  m_runManager->BeamOn(m_cfg.testsPerEvent);

  auto particleProperties
      = FW::G4::FVRunAction::Instance()->firstAndLastProperties();

  // write particle properties to event store
  if (m_cfg.particlePropertiesCollection != ""
      && context.eventStore.add(m_cfg.particlePropertiesCollection,
                                std::move(particleProperties))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  return FW::ProcessCode::SUCCESS;
}
