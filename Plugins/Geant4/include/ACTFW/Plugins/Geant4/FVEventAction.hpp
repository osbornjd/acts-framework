// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_GEANT4_FVEVENTACTION_H
#define ACTFW_PLUGINS_GEANT4_FVEVENTACTION_H 1

#include <memory>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "G4UserEventAction.hh"
#include "globals.hh"

namespace FW {
namespace G4 {

  /// @class FVEventAction
  ///
  /// @brief Geant4 event action for Fatras validation
  ///
  /// This event action can be used to validate the Fatras simulation.
  /// If used with its corresponding stepping action (FW::G4::FVSteppingAction)
  /// and
  /// FW::G4::PrimaryGeneratorAction it returns the Acts::ParticleProperties of
  /// the
  /// first step and the last step (when the radial step limit is reached).

  class FVEventAction : public G4UserEventAction
  {
  public:
    /// Constructor
    /// @param The number of particles for this event
    FVEventAction(size_t nParticles);
    /// Destructor
    virtual ~FVEventAction();
    /// Static access method
    static FVEventAction*
    Instance();
    /// Interface method for begin of the event
    /// @note resets the material step action
    virtual void
    BeginOfEventAction(const G4Event* event);
    /// Interface method for end of event
    /// @note this method is writing out the material track records
    virtual void
    EndOfEventAction(const G4Event* event);
    /// Interface method
    /// @note does nothing
    void
    Reset();

    /// @return A vector of all corresponding first and last
    /// Acts::ParticleProperties of this event
    const std::
        vector<std::pair<std::pair<Acts::ParticleProperties, Acts::Vector3D>,
                         std::pair<Acts::ParticleProperties, Acts::Vector3D>>>&
        firstAndLastProperties()
    {
      return m_particles;
    }

  private:
    /// Instance of the FVEventAction
    static FVEventAction* fgInstance;
    /// To collect the first and last particle properties
    std::vector<std::pair<std::pair<Acts::ParticleProperties, Acts::Vector3D>,
                          std::pair<Acts::ParticleProperties, Acts::Vector3D>>>
        m_particles;
  };
}  // end of namespace G4
}  // end of namespace FW

#endif  // ACTFW_PLUGINS_GEANT4_FVEVENTACTION_H
