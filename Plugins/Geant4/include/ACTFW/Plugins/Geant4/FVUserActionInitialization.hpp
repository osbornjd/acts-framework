///////////////////////////////////////////////////////////////////
// FVUserActionInitialization.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_PLUGINS_GEANT4_FVUSERACTIONINITIALIZATION_H
#define ACTFW_PLUGINS_GEANT4_FVUSERACTIONINITIALIZATION_H 1

#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "G4VUserActionInitialization.hh"

namespace FW {
namespace G4 {
  /// @class FVUserActionInitialization
  ///
  /// @brief Geant4 user action initialization for Fatras validation
  ///
  /// Initializes all needed user action for validation of the Fatras
  /// simulation:
  ///
  /// - FWG4::PrimaryGeneratorAction
  /// - FWG4::FVRunAction
  /// - FWG4::FVEventAction
  /// - FVSteppingAction(m_radialStepLimit
  ///

  class FVUserActionInitialization : public G4VUserActionInitialization
  {
  public:
    /// Constructor
    FVUserActionInitialization(FWG4::PrimaryGeneratorAction::Config pgaConfig,
                               double radialStepLimit);
    /// Destructor
    virtual ~FVUserActionInitialization() = default;
    /// Virtual method to define user action classes for worker threads and for
    /// sequential mode
    virtual void
    Build() const override final;
    /// Virtual to define user action classes for the master thread
    virtual void
    BuildForMaster() const override final;

  private:
    /// The configuration object for the PrimaryGeneratorAction
    PrimaryGeneratorAction::Config m_pgaConfig;
    /// The radial step limit
    double m_radialStepLimit;
  };
}  // end of namespace G4
}  // end of namespace FW

#endif  // ACTFW_PLUGINS_GEANT4_FVUSERACTIONINITIALIZATION_H
