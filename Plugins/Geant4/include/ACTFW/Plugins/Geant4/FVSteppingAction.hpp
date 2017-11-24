// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_GEANT4_FVSTEPPINGACTION_H
#define ACTFW_PLUGINS_GEANT4_FVSTEPPINGACTION_H 1

#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"
#include "G4UserSteppingAction.hh"
#include "globals.hh"

namespace FW {
namespace G4 {

  /// @class FVSteppingAction
  ///
  /// @brief Geant4 stepping action for Fatras validation
  ///
  /// This stepping action can be used to validate the Fatras simulation.
  /// It hands back the Acts::ParticleProperties at a given radial limit.

  class FVSteppingAction : public G4UserSteppingAction
  {
  public:
    /// Constructor
    /// @param radialStepLimit The radial step limit at which
    /// Acts::ParticleProperties want to be obtained
    FVSteppingAction(double radialStepLimit);
    /// Destructor
    virtual ~FVSteppingAction();

    /// Static access method
    static FVSteppingAction*
    Instance();

    /// Interface Method doing the step
    /// @note it creates and collects the MaterialStep entities
    virtual void
    UserSteppingAction(const G4Step*);

    /// Interface reset method
    /// @note it clears the collected step vector
    void
    Reset();

    /// @return The Acts::ParticleProperties at the given radial step limit
    const std::pair<Acts::ParticleProperties, Acts::Vector3D>&
    particleAtStepLimit()
    {
      return m_particleAtStepLimit;
    }

  private:
    /// Instance of the FVSteppingAction
    static FVSteppingAction* fgInstance;
    /// The step Limit
    double m_radialStepLimit;
    /// The collected steps
    std::pair<Acts::ParticleProperties, Acts::Vector3D> m_particleAtStepLimit;
    ///  Flag indicating if step limit was reached
    bool m_reachedStepLimit;
  };
}  // end of namespace G4
}  // end of namespace FW

#endif  // ACTFW_PLUGINS_GEANT4_FVSTEPPINGACTION_H
