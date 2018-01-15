// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//---------------------------------------------------------------------------
//
// ClassName:   G4EmStandardPhysics
//
// Author:      V.Ivanchenko 09.11.2005
//
// Modified:
// 05.12.2005 V.Ivanchenko add controlled verbosity
// 23.11.2006 V.Ivanchenko remove mscStepLimit option and improve cout
//
//----------------------------------------------------------------------------
//
// This class provides construction of default EM standard physics
//

#ifndef G4EmStandardPhysics_h
#define G4EmStandardPhysics_h 1

#include "G4VPhysicsConstructor.hh"
#include "globals.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// @todo
// This physics list was copied from Geant4 to validate Fatras.
// It contains the electromagnetic processes. Currently for the muon, which was
// being tested, only multiple scattering and energy loss are turned on.

namespace FW {
namespace G4 {

  class G4EmStandardPhysics : public G4VPhysicsConstructor
  {
  public:
    G4EmStandardPhysics(G4int ver = 0);

    // obsolete
    G4EmStandardPhysics(G4int ver, const G4String& name);

    virtual ~G4EmStandardPhysics();

    virtual void
    ConstructParticle();
    virtual void
    ConstructProcess();

  private:
    G4int verbose;
  };
}  // end of namespace G4
}  // end of namespace FW

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
