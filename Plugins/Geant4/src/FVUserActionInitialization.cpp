// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Geant4/FVUserActionInitialization.hpp"
#include "ACTFW/Plugins/Geant4/FVEventAction.hpp"
#include "ACTFW/Plugins/Geant4/FVRunAction.hpp"
#include "ACTFW/Plugins/Geant4/FVSteppingAction.hpp"

FW::G4::FVUserActionInitialization::FVUserActionInitialization(
    FW::G4::PrimaryGeneratorAction::Config pgaConfig,
    double                                 radialStepLimit)
  : G4VUserActionInitialization()
  , m_pgaConfig(pgaConfig)
  , m_radialStepLimit(radialStepLimit)
{
}

void
FW::G4::FVUserActionInitialization::Build() const
{
  SetUserAction(new FW::G4::PrimaryGeneratorAction(m_pgaConfig));
  SetUserAction(new FW::G4::FVRunAction());
  SetUserAction(new FW::G4::FVEventAction(m_pgaConfig.nParticles));
  SetUserAction(new FW::G4::FVSteppingAction(m_radialStepLimit));
}

void
FW::G4::FVUserActionInitialization::BuildForMaster() const
{
  SetUserAction(new FW::G4::FVRunAction());
}
