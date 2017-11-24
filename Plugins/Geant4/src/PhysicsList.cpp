// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Geant4/PhysicsList.hpp"
#include "ACTFW/Plugins/Geant4/G4EmStandardPhysics.hpp"

FW::G4::PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  SetVerboseLevel(1);

  // EM physics
  RegisterPhysics(new G4EmStandardPhysics());
}

FW::G4::PhysicsList::~PhysicsList()
{
}

void
FW::G4::PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();
}
