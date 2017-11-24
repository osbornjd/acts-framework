// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "G4AutoLock.hh"
#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

namespace {
G4Mutex myLowEPrimGenMutex = G4MUTEX_INITIALIZER;
}

FW::G4::PrimaryGeneratorAction* FW::G4::PrimaryGeneratorAction::fgInstance = 0;

FW::G4::PrimaryGeneratorAction::PrimaryGeneratorAction(const Config& cfg)
  : G4VUserPrimaryGeneratorAction()
  , m_cfg(cfg)
  , m_particleGun(new G4ParticleGun(m_cfg.nParticles))
  , m_particle(
        G4ParticleTable::GetParticleTable()->FindParticle(m_cfg.particleName))
  , m_position(0., 0., 0.)
  , m_direction(0., 0., 0.)
{
  // configure the run
  fgInstance = this;

  m_particleGun->SetParticleDefinition(m_particle);
  m_particleGun->SetParticleEnergy(m_cfg.energy);
}

FW::G4::PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  fgInstance = 0;
  delete m_particleGun;
}

FW::G4::PrimaryGeneratorAction*
FW::G4::PrimaryGeneratorAction::Instance()
{
  // Static acces function via G4RunManager

  return fgInstance;
}

const std::pair<Acts::ParticleProperties, Acts::Vector3D>&
FW::G4::PrimaryGeneratorAction::primaryParticleProperties() const
{
  Acts::Vector3D position(m_position.x(), m_position.y(), m_position.z());

  G4ThreeVector g4momentum = m_particleGun->GetParticleMomentum()
      * m_particleGun->GetParticleMomentumDirection();
  Acts::Vector3D momentum(g4momentum.x(), g4momentum.y(), g4momentum.z());

  double mass   = m_particle->GetPDGMass();
  double charge = m_particle->GetPDGCharge();
  /// @todo translate particle type and barcode from ParticleDefition in
  /// geant4
  ///
  return std::make_pair(Acts::ParticleProperties(momentum, mass, charge),
                        position);
}

void
FW::G4::PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // set the random seeds
  CLHEP::RanluxEngine defaultEngine(anEvent->GetEventID());
  G4Random::setTheEngine(&defaultEngine);
  G4Random::setTheSeed(m_cfg.randomSeed);
  // this function is called at the begining of event
  G4double phi = m_cfg.phiRange.at(0)
      + G4UniformRand() * fabs(m_cfg.phiRange.at(1) - m_cfg.phiRange.at(0));
  G4double theta = m_cfg.etaRange.at(0)
      + G4UniformRand() * fabs(m_cfg.etaRange.at(1) - m_cfg.etaRange.at(0));
  // build a direction
  m_direction
      = G4ThreeVector(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
  /// position
  m_position = G4ThreeVector(
      0., 0., G4RandGauss::shoot(m_cfg.zPosRange.at(0), m_cfg.zPosRange.at(1)));
  /// pT
  G4double pT = m_cfg.ptRange.at(0)
      + G4UniformRand() * fabs(m_cfg.ptRange.at(1) - m_cfg.ptRange.at(0));

  // set to the particle gun and
  m_particleGun->SetParticleMomentum(pT * m_direction);
  m_particleGun->SetParticlePosition(m_position);
  m_particleGun->GeneratePrimaryVertex(anEvent);
}
