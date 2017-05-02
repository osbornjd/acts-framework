///////////////////////////////////////////////////////////////////
// MMPrimaryGeneratorAction.h
///////////////////////////////////////////////////////////////////

#ifndef GEANT4MATERIALMAPPING_MMPRIMARYGENERATORACTION_H
#define GEANT4MATERIALMAPPING_MMPRIMARYGENERATORACTION_H 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;

namespace G4MM {
    
    /// @class MMPrimaryGeneratorAction
    ///
    /// @brief configures the run
    ///
    /// The MMPrimaryGeneratorAction is the implementation of the Geant4
    /// class G4VUserPrimaryGeneratorAction. It generates a random direction
    /// and shoots a geantino.
    ///
    
    class MMPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
    {
    public:
        /// Constructor
        MMPrimaryGeneratorAction(const G4String& particleName = "geantino",
                                 G4double energy = 1.*MeV,
                                 G4ThreeVector position= G4ThreeVector(0.,0.,0.),
                                 G4ThreeVector momentumDirection = G4ThreeVector(0.,0.,1.));
        /// Destructor
        ~MMPrimaryGeneratorAction();
        /// Static access method
        static MMPrimaryGeneratorAction* Instance();
        /// Interface method to generate the primary
        virtual void GeneratePrimaries(G4Event*);
        /// Access method to get the initial direction
        G4ThreeVector direction() {return m_dir;}
        /// Access method to get the start position
        G4ThreeVector startPosition() {return m_startPos;}
        
    private:
        /// Instance of the PrimaryGeneratorAction
        static MMPrimaryGeneratorAction* fgInstance;
        /// Pointer to the G4 particle gun
        G4ParticleGun*  fParticleGun;
        /// Initial direction
        G4ThreeVector   m_dir;
        /// Start position of the track
        G4ThreeVector   m_startPos;
    };

}

#endif //GEANT4MATERIALMAPPING_MMPRIMARYGENERATORACTION_H
