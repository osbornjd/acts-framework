#include "ACTFW/Geant4MaterialMapping/MMEventAction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMPrimaryGeneratorAction.hpp"
#include "ACTFW/Geant4MaterialMapping/MMSteppingAction.hpp"
#include "G4Event.hh"
#include "G4RunManager.hh"

G4MM::MMEventAction* G4MM::MMEventAction::fgInstance = 0;

G4MM::MMEventAction*
G4MM::MMEventAction::Instance()
{
  // Static acces function via G4RunManager

  return fgInstance;
}

G4MM::MMEventAction::MMEventAction() : G4UserEventAction()
{
  fgInstance = this;
}

G4MM::MMEventAction::~MMEventAction()
{
  fgInstance = 0;
}

void
G4MM::MMEventAction::BeginOfEventAction(const G4Event* event)
{
  // reset the collection of material steps
  MMSteppingAction::Instance()->Reset();
}

void
G4MM::MMEventAction::EndOfEventAction(const G4Event* event)
{
  Acts::MaterialStep::Position pos(event->GetPrimaryVertex()->GetX0(),
                                   event->GetPrimaryVertex()->GetY0(),
                                   event->GetPrimaryVertex()->GetZ0());
  // access the initial direction of the track
  G4ThreeVector dir   = MMPrimaryGeneratorAction::Instance()->direction();
  double        theta = dir.theta();
  double        phi   = dir.phi();
  // create the MaterialTrackRecord
  Acts::MaterialTrackRecord mtrecord(
      pos, theta, phi, MMSteppingAction::Instance()->materialSteps());
  // write out the MaterialTrackRecord of one event
  m_records.push_back(mtrecord);
}

void
G4MM::MMEventAction::Reset()
{
}
