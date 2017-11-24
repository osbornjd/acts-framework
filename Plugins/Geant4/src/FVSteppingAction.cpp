// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Geant4/FVSteppingAction.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "G4Material.hh"
#include "G4Step.hh"

FW::G4::FVSteppingAction* FW::G4::FVSteppingAction::fgInstance = 0;

FW::G4::FVSteppingAction*
FW::G4::FVSteppingAction::Instance()
{
  // Static acces function via G4RunManager

  return fgInstance;
}

FW::G4::FVSteppingAction::FVSteppingAction(double radialStepLimit)
  : G4UserSteppingAction()
  , m_radialStepLimit(radialStepLimit)
  , m_particleAtStepLimit(
        std::make_pair(Acts::ParticleProperties(Acts::Vector3D(0., 0., 0.)),
                       Acts::Vector3D(0., 0., 0.)))
  , m_reachedStepLimit(false)
{
  fgInstance = this;
}

FW::G4::FVSteppingAction::~FVSteppingAction()
{
  fgInstance = 0;
}

void
FW::G4::FVSteppingAction::UserSteppingAction(const G4Step* step)
{
  // Get the first step reaching the step limit
  if (step->GetPostStepPoint()->GetPosition().r() >= m_radialStepLimit
      && !m_reachedStepLimit) {
    Acts::Vector3D position(step->GetPostStepPoint()->GetPosition().x(),
                            step->GetPostStepPoint()->GetPosition().y(),
                            step->GetPostStepPoint()->GetPosition().z());
    Acts::Vector3D momentum(step->GetPostStepPoint()->GetMomentum().x(),
                            step->GetPostStepPoint()->GetMomentum().y(),
                            step->GetPostStepPoint()->GetMomentum().z());

    double mass   = step->GetPostStepPoint()->GetMass();
    double charge = step->GetPostStepPoint()->GetCharge();

    m_reachedStepLimit = true;

    m_particleAtStepLimit = std::make_pair(
        Acts::ParticleProperties(momentum, mass, charge), position);
  }
}

void
FW::G4::FVSteppingAction::Reset()
{
  m_particleAtStepLimit
      = std::make_pair(Acts::ParticleProperties(Acts::Vector3D(0., 0., 0.)),
                       Acts::Vector3D(0., 0., 0.));
  m_reachedStepLimit = false;
}
