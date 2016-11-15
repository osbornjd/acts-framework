#include "ACTFW/Geant4MaterialMapping/MMSteppingAction.hpp"
#include "G4Material.hh"
#include "G4Step.hh"

G4MM::MMSteppingAction* G4MM::MMSteppingAction::fgInstance = 0;

G4MM::MMSteppingAction*
G4MM::MMSteppingAction::Instance()
{
  // Static acces function via G4RunManager

  return fgInstance;
}

G4MM::MMSteppingAction::MMSteppingAction() : G4UserSteppingAction(), m_steps()
// m_volMgr(MaterialRunAction::Instance()->getGeant4VolumeManager())
{
  fgInstance = this;
}

G4MM::MMSteppingAction::~MMSteppingAction()
{
  fgInstance = 0;
}

void
G4MM::MMSteppingAction::UserSteppingAction(const G4Step* step)
{
  // get the material
  G4Material* material = step->GetPreStepPoint()->GetMaterial();
  if (material && material->GetName() != "Vacuum"
      && material->GetName() != "Air") {
    // go through the elements of the material & weigh it with its fraction
    const G4ElementVector* elements  = material->GetElementVector();
    const G4double*        fraction  = material->GetFractionVector();
    size_t                 nElements = material->GetNumberOfElements();
    double                 A         = 0.;
    double                 Z         = 0.;
    double                 X0        = material->GetRadlen();
    double                 L0        = material->GetNuclearInterLength();
    double rho        = material->GetDensity() * CLHEP::mm3 / CLHEP::gram;
    double steplength = step->GetStepLength() / CLHEP::mm3;
    if (nElements == 1) {
      A = material->GetA() * CLHEP::mole / CLHEP::gram;
      Z = material->GetZ();
    } else {
      for (size_t i = 0; i < nElements; i++) {
        A += elements->at(i)->GetA() * fraction[i];
        Z += elements->at(i)->GetZ() * fraction[i];
      }
      if (A != 0.) A /= nElements;
      if (A != 0.) Z /= nElements;
    }

    G4cout << *material << G4endl;
    G4cout << "----G4StepMaterial----" << G4endl;
    /// @TODO remove output after testing
    G4cout << "Material: " << material->GetName() << G4endl;
    G4cout << "X0: " << X0 << G4endl;
    G4cout << "L0: " << L0 << G4endl;
    G4cout << "A: " << A << G4endl;
    G4cout << "Z: " << Z << G4endl;
    G4cout << "rho: " << rho << G4endl;
    G4cout << "steplength: " << steplength << G4endl;

    // create the Materialstep
    const Acts::MaterialStep::Position pos(
        step->GetPreStepPoint()->GetPosition().x(),
        step->GetPreStepPoint()->GetPosition().y(),
        step->GetPreStepPoint()->GetPosition().z());
    const Acts::MaterialProperties matprop(steplength, X0, L0, A, Z, rho);
    Acts::MaterialStep             mstep = Acts::MaterialStep(matprop, pos);
    m_steps.push_back(mstep);
  }
}

void
G4MM::MMSteppingAction::Reset()
{
  m_steps.clear();
}
