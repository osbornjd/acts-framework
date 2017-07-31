///////////////////////////////////////////////////////////////////
// PrimaryGeneratorAction.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_GEANT4_PRIMARYGENERATORACTION_H
#define ACTFW_PLUGINS_GEANT4_PRIMARYGENERATORACTION_H

#include <array>
#include "ACTS/Utilities/Units.hpp"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

class G4ParticleGun;
class G4Event;
class G4ParticleDefinition;

namespace Acts {
class ParticleProperties;
}

namespace FW {
namespace G4 {

  /// @class PrimaryGeneratorAction
  ///
  /// @brief configures the run
  ///
  /// The PrimaryGeneratorAction is the implementation of the Geant4
  /// class G4VUserPrimaryGeneratorAction. It generates a random direction
  /// and shoots a particle.
  ///
  /// @todo tempate with RandomService
  class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
  {
  public:
    /// @struct Config Configuration object of the PrimaryGeneratorAction
    struct Config
    {
      /// Number of particles
      size_t nParticles = 1;
      /// The name of the particle (should be recignizable by Geant4)
      std::string particleName = "geantino";
      /// The energy of the particle
      double energy = 1000. * Acts::units::_MeV;
      /// low, high for eta range
      std::array<double, 2> etaRange = {{-3., 3.}};
      /// low, high for phi range
      std::array<double, 2> phiRange = {{-M_PI, M_PI}};
      /// low, high for pt range
      std::array<double, 2> ptRange
          = {{100. * Acts::units::_MeV, 10000. * Acts::units::_MeV}};
      /// low, high z range for the position
      std::array<double, 2> zPosRange
          = {{-100. * Acts::units::_mm, 100. * Acts::units::_mm}};
      /// random seed
      int randomSeed = 123456789;
    };
    /// Constructor
    PrimaryGeneratorAction(const Config& cfg);
    /// Destructor
    ~PrimaryGeneratorAction();

    /// Static access method
    static PrimaryGeneratorAction*
    Instance();

    /// Interface method to generate the primary
    virtual void
    GeneratePrimaries(G4Event*);

    /// Access method to get the initial direction
    G4ThreeVector
    direction()
    {
      return m_direction;
    }

    /// Access method to get the start position
    G4ThreeVector
    startPosition()
    {
      return m_position;
    }

    /// Access method to primary particle properties
    Acts::ParticleProperties
    primaryParticleProperties() const;

  private:
    /// Instance of the PrimaryGeneratorAction
    static PrimaryGeneratorAction* fgInstance;

    /// The internal configuration object
    Config m_cfg;
    /// Pointer to the G4 particle gun
    G4ParticleGun* m_particleGun;
    /// the primary particle
    G4ParticleDefinition* m_particle;
    /// position to be returned
    G4ThreeVector m_position;
    /// direction to be returned
    G4ThreeVector m_direction;
  };
}  // end of namespace G4
}  // end of namespace FW

#endif  // GEANT4MATERIALMAPPING_PRIMARYGENERATORACTION_H
