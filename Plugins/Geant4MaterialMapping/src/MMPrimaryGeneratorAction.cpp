#include "ACTFW/Geant4MaterialMapping/MMPrimaryGeneratorAction.hpp"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4RandomDirection.hh"
#include "G4UnitsTable.hh"
#include "Randomize.hh"

G4MM::MMPrimaryGeneratorAction* G4MM::MMPrimaryGeneratorAction::fgInstance = 0;

G4MM::MMPrimaryGeneratorAction::MMPrimaryGeneratorAction(
    const G4String& particleName,
    G4double        energy,
    G4ThreeVector   position,
    G4ThreeVector   momentumDirection)
  : G4VUserPrimaryGeneratorAction()
  , fParticleGun(0)
  , m_dir(momentumDirection)
  , m_startPos(position)
{
  // configure the run
  fgInstance         = this;
  G4int nofParticles = 1;
  fParticleGun       = new G4ParticleGun(nofParticles);
  // default particle kinematic
  G4ParticleTable*      particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
  fParticleGun->SetParticleDefinition(particle);
  fParticleGun->SetParticleEnergy(energy);
  fParticleGun->SetParticlePosition(position);
  //  m_dir = momentumDirection;
  G4UnitDefinition::PrintUnitsTable();
}

G4MM::MMPrimaryGeneratorAction::~MMPrimaryGeneratorAction()
{
  fgInstance = 0;
  delete fParticleGun;
}

G4MM::MMPrimaryGeneratorAction*
G4MM::MMPrimaryGeneratorAction::Instance()
{
  // Static acces function via G4RunManager

  return fgInstance;
}

void
G4MM::MMPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // this function is called at the begining of event

  // generate random direction
  G4double phi   = -M_PI + G4UniformRand() * 2. * M_PI;
  G4double theta = G4UniformRand() * M_PI;

  m_dir
      = G4ThreeVector(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));

  /* G4cout << "___________________________________" << G4endl;
   G4cout << " new direction: " << m_dir << G4endl;
   G4cout << "Event ID: " << anEvent->GetEventID() << G4endl;
   G4cout << "___________________________________" << G4endl;*/

  fParticleGun->SetParticleMomentumDirection(m_dir);

  fParticleGun->GeneratePrimaryVertex(anEvent);
}
