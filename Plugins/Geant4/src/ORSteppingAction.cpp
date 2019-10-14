// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Geant4/ORSteppingAction.hpp"
#include "ACTFW/Plugins/Geant4/OREventAction.hpp"
#include <stdexcept>
#include "Acts/Utilities/Units.hpp"
#include "G4Step.hh"
#include "G4VProcess.hh"
//~ #include "SystemOfUnits.h"

FW::Geant4::ORSteppingAction* FW::Geant4::ORSteppingAction::fgInstance
    = nullptr;

FW::Geant4::ORSteppingAction*
FW::Geant4::ORSteppingAction::Instance()
{
  // Static acces function via G4RunManager
  return fgInstance;
}

FW::Geant4::ORSteppingAction::ORSteppingAction()
  : G4UserSteppingAction()
{
  if (fgInstance) {
    throw std::logic_error("Attempted to duplicate a singleton");
  } else {
    fgInstance = this;
  }
}

FW::Geant4::ORSteppingAction::ORSteppingAction(OREventAction* eventAction) : m_EventAction(eventAction)
{
}


FW::Geant4::ORSteppingAction::~ORSteppingAction()
{
  fgInstance = nullptr;
}

void
FW::Geant4::ORSteppingAction::UserSteppingAction(const G4Step* step)
{	
	ParticleRecord p;
	p.position[0] = step->GetPostStepPoint()->GetPosition().x() / CLHEP::mm;
	p.position[1] = step->GetPostStepPoint()->GetPosition().y() / CLHEP::mm;
	p.position[2] = step->GetPostStepPoint()->GetPosition().z() / CLHEP::mm;
	p.momentum[0] = step->GetPostStepPoint()->GetMomentum().x() / CLHEP::GeV;
	p.momentum[1] = step->GetPostStepPoint()->GetMomentum().y() / CLHEP::GeV;
	p.momentum[2] = step->GetPostStepPoint()->GetMomentum().z() / CLHEP::GeV;
	p.globalTime = step->GetPostStepPoint()->GetGlobalTime();
	p.pdg = step->GetTrack()->GetDynamicParticle()->GetPDGcode();
	p.vertex[0] = step->GetTrack()->GetVertexPosition().x();
	p.vertex[1] = step->GetTrack()->GetVertexPosition().y();
	p.vertex[2] = step->GetTrack()->GetVertexPosition().z();
	p.energy = step->GetPostStepPoint()->GetTotalEnergy() / CLHEP::GeV;
	p.mass = step->GetPostStepPoint()->GetMass();
	p.charge = step->GetPostStepPoint()->GetCharge();
	p.trackid = step->GetTrack()->GetTrackID();
	p.parentid = step->GetTrack()->GetParentID();
	p.volume = (step->GetPostStepPoint()->GetPhysicalVolume() != nullptr) ? step->GetPostStepPoint()->GetPhysicalVolume()->GetName() : "No volume";
	p.process = step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
	m_EventAction->AddParticle(p);
}

void
FW::Geant4::ORSteppingAction::Reset()
{
}
