#include "ACTFW/Plugins/Geant4/FVEventAction.hpp"
#include "ACTFW/Plugins/Geant4/FVSteppingAction.hpp"
#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "G4Event.hh"
#include "G4RunManager.hh"

FW::G4::FVEventAction* FW::G4::FVEventAction::fgInstance = 0;

FW::G4::FVEventAction*
FW::G4::FVEventAction::Instance()
{
  // Static acces function viaFW::G4RunManager

  return fgInstance;
}

FW::G4::FVEventAction::FVEventAction(size_t nParticles)
  : G4UserEventAction(), m_particles()
{
  m_particles.reserve(nParticles);
  fgInstance = this;
}

FW::G4::FVEventAction::~FVEventAction()
{
  fgInstance = 0;
}

void
FW::G4::FVEventAction::BeginOfEventAction(const G4Event* event)
{
  FW::G4::FVSteppingAction::Instance()->Reset();
}

void
FW::G4::FVEventAction::EndOfEventAction(const G4Event* event)
{
  // store particle properties of last and first particle
  auto lastProperties
      = FW::G4::FVSteppingAction::Instance()->particleAtStepLimit();
  auto primaryProperties
      = FW::G4::PrimaryGeneratorAction::Instance()->primaryParticleProperties();
  m_particles.push_back(std::make_pair(primaryProperties, lastProperties));
  FW::G4::FVSteppingAction::Instance()->Reset();
}

void
FW::G4::FVEventAction::Reset()
{
}
