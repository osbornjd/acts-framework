#include "ACTFW/Geant4MaterialMapping/MMRunAction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMEventAction.hpp"
#include "G4Run.hh"

G4MM::MMRunAction* G4MM::MMRunAction::fgInstance = 0;


G4MM::MMRunAction::MMRunAction()
: G4UserRunAction()
{
    fgInstance = this;
}

G4MM::MMRunAction::~MMRunAction()
{
    fgInstance = 0;
}

G4MM::MMRunAction* G4MM::MMRunAction::Instance()
{
    return fgInstance;
}

void G4MM::MMRunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
    //initialize event cumulative quantities
    MMEventAction::Instance()->Reset();
}

void G4MM::MMRunAction::EndOfRunAction(const G4Run* aRun)
{
    G4int nofEvents = aRun->GetNumberOfEvent();
    if (nofEvents == 0) return;
    
    // Print
    G4cout
    << "\n--------------------End of Run------------------------------\n"
    << "\n------------------------------------------------------------\n"
    << G4endl;
    
}

