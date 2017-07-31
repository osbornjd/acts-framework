
#include "ACTFW/Plugins/Geant4/PhysicsList.hpp"
#include "ACTFW/Plugins/Geant4/G4EmStandardPhysics.hpp"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FW::G4::PhysicsList::PhysicsList() : G4VModularPhysicsList()
{
  SetVerboseLevel(1);

  // EM physics
  RegisterPhysics(new G4EmStandardPhysics());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

FW::G4::PhysicsList::~PhysicsList()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void
FW::G4::PhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();
}
