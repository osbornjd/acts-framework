///////////////////////////////////////////////////////////////////
// MMPrimaryGeneratorAction.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_GEANT4_MMPRIMARYGENERATORACTION_H
#define ACTFW_PLUGINS_GEANT4_MMPRIMARYGENERATORACTION_H

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"

class G4ParticleGun;
class G4Event;

namespace FWG4 {
    
    /// @class MMPrimaryGeneratorAction
    ///
    /// @brief configures the run
    ///
    /// The MMPrimaryGeneratorAction is the implementation of the Geant4
    /// class G4VUserPrimaryGeneratorAction. It generates a random direction
    /// and shoots a geantino.
    ///
    /// @todo tempate with RandomService
    class MMPrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
    {
    public:
        /// Constructor
        MMPrimaryGeneratorAction(const G4String& particleName = "geantino",
                                 G4double energy = 1000.*MeV,
                                 G4int randomSeed1 = 12345,
                                 G4int randomSeed2 = 23456);
        /// Destructor
        ~MMPrimaryGeneratorAction();
        
        /// Static access method
        static MMPrimaryGeneratorAction* Instance();
        
        /// Interface method to generate the primary
        virtual void GeneratePrimaries(G4Event*);
        
        /// Access method to get the initial direction
        G4ThreeVector direction() {return m_direction;}
        
        /// Access method to get the start position
        G4ThreeVector startPosition() {return m_position;}
        
    private:
        /// Instance of the PrimaryGeneratorAction
        static MMPrimaryGeneratorAction* fgInstance;
        
        /// Pointer to the G4 particle gun
        G4ParticleGun*  fParticleGun;
        
        /// position to be returned 
        G4ThreeVector   m_position;
        /// direction to be returned
        G4ThreeVector   m_direction;
  
    };

}

#endif //GEANT4MATERIALMAPPING_MMPRIMARYGENERATORACTION_H
