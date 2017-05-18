#include "ACTFW/Plugins/Geant4/MMRunAction.hpp"
#include "ACTFW/Plugins/Geant4/MMEventAction.hpp"
#include "G4Run.hh"

FWG4::MMRunAction* FWG4::MMRunAction::fgInstance = 0;


FWG4::MMRunAction::MMRunAction()
: G4UserRunAction()
{
    fgInstance = this;
}

FWG4::MMRunAction::~MMRunAction()
{
    fgInstance = 0;
}

FWG4::MMRunAction* FWG4::MMRunAction::Instance()
{
    return fgInstance;
}

void FWG4::MMRunAction::BeginOfRunAction(const G4Run* aRun)
{
    G4cout << "### Run " << aRun->GetRunID() << " start." << G4endl;
    //initialize event cumulative quantities
    MMEventAction::Instance()->Reset();
}

void FWG4::MMRunAction::EndOfRunAction(const G4Run* aRun)
{
    G4int nofEvents = aRun->GetNumberOfEvent();
    if (nofEvents == 0) return;
    
    // Print
    G4cout
    << "\n--------------------End of Run------------------------------\n"
    << "\n------------------------------------------------------------\n"
    << G4endl;
    
}

