// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Geant4/FVRunAction.hpp"
#include "ACTFW/Plugins/Geant4/FVEventAction.hpp"
#include "G4Run.hh"

FW::G4::FVRunAction* FW::G4::FVRunAction::fgInstance = 0;

FW::G4::FVRunAction::FVRunAction() : G4UserRunAction(), m_particleProperties()
{
  fgInstance = this;
}

FW::G4::FVRunAction::~FVRunAction()
{
  fgInstance = 0;
}

FW::G4::FVRunAction*
FW::G4::FVRunAction::Instance()
{
  return fgInstance;
}

void
FW::G4::FVRunAction::BeginOfRunAction(const G4Run* aRun)
{
  G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
}

void
FW::G4::FVRunAction::EndOfRunAction(const G4Run* aRun)
{
  G4int nofEvents = aRun->GetNumberOfEvent();
  if (nofEvents == 0) return;

  auto particleProperties
      = FW::G4::FVEventAction::Instance()->firstAndLastProperties();
  m_particleProperties.insert(m_particleProperties.end(),
                              particleProperties.begin(),
                              particleProperties.end());

  // Print
  G4cout << "\n--------------------End of Run------------------------------\n"
         << "\n------------------------------------------------------------\n"
         << G4endl;
}
