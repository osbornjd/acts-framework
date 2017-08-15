#include "ACTFW/Plugins/Geant4/MMEventAction.hpp"
#include "ACTFW/Plugins/Geant4/MMPrimaryGeneratorAction.hpp"
#include "ACTFW/Plugins/Geant4/MMSteppingAction.hpp"
#include "G4Event.hh"
#include "G4RunManager.hh"

FW::G4::MMEventAction* FW::G4::MMEventAction::fgInstance = 0;

FW::G4::MMEventAction*
FW::G4::MMEventAction::Instance()
{
  // Static acces function via G4RunManager

  return fgInstance;
}

FW::G4::MMEventAction::MMEventAction() : G4UserEventAction()
{
  fgInstance = this;
}

FW::G4::MMEventAction::~MMEventAction()
{
  fgInstance = 0;
}

void
FW::G4::MMEventAction::BeginOfEventAction(const G4Event*)
{
  // reset the collection of material steps
  MMSteppingAction::Instance()->Reset();
}

void
FW::G4::MMEventAction::EndOfEventAction(const G4Event* event)
{
  Acts::MaterialStep::Position pos(event->GetPrimaryVertex()->GetX0(),
                                   event->GetPrimaryVertex()->GetY0(),
                                   event->GetPrimaryVertex()->GetZ0());
  // access the initial direction of the track
  G4ThreeVector dir   = MMPrimaryGeneratorAction::Instance()->direction();
  double        theta = dir.theta();
  double        phi   = dir.phi();
  // loop over the material steps and add up the material
  double tX0 = 0;
  double tL0 = 0;
  for (auto& mstep : MMSteppingAction::Instance()->materialSteps()){
    tX0 += mstep.materialProperties().thicknessInX0(); 
    tL0 += mstep.materialProperties().thicknessInL0();
  }  
  // create the MaterialTrack
  Acts::MaterialTrack mtrecord(
      pos, 
      theta, phi, 
      MMSteppingAction::Instance()->materialSteps(),
      tX0, tL0);
  // write out the MaterialTrack of one event
  m_records.push_back(mtrecord);
}

void
FW::G4::MMEventAction::Reset()
{
}
