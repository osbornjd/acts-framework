// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>
#include <vector>
#include "ACTFW/Barcode/Barcode.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Utilities/Units.hpp"

namespace FW {

/// Typedef the pdg code
typedef int pdg_type;

namespace Data {

  /// @brief Modifier of particles
  class SimParticleModifier
  {

  public:
    /// @brief Default Constructor
    SimParticleModifier() = default;

    /// @brief Set the limits
    ///
    /// @param [in, out] particle Particle that is manipulated
    /// @param [in] x0Limit the limit in X0 to be passed
    /// @param [in] l0Limit the limit in L0 to be passed
    /// @param [in] timeLimit the readout time limit to be passed
    void
    setLimits(SimParticle& particle,
              double       x0Limit,
              double       l0Limit,
              double       timeLimit = std::numeric_limits<double>::max())
    {
      particle.m_limitInX0 = x0Limit;
      particle.m_limitInL0 = l0Limit;
      particle.m_timeLimit = timeLimit;
    }

    /// @brief Place the particle in the detector and set barcode
    ///
    /// @param [in, out] particle Particle that is manipulated
    /// @param [in] positon Position of the particle
    /// @param [in] barcode Barcode of the particle
    /// @param [in] timeStamp Age of the particle
    void
    place(SimParticle&   particle,
          Acts::Vector3D position,
          barcode_type   barcode,
          double         timeStamp = 0.)
    {
      particle.m_position  = std::move(position);
      particle.m_barcode   = barcode;
      particle.m_timeStamp = timeStamp;
    }

    /// @brief Update the particle with applying scattering
    ///
    /// @param [in, out] particle Particle that is manipulated
    /// @param [in] nmomentum New momentum of the particle
    void
    scatter(SimParticle& particle, Acts::Vector3D& nmomentum)
    {
      particle.m_momentum = std::move(nmomentum);
      particle.m_pT       = particle.m_momentum.perp();
    }

    /// @brief Update the particle with applying energy loss
    ///
    /// @param [in, out] particle Particle that is manipulated
    /// @param [in] deltaE is the energy loss to be applied
    void
    energyLoss(SimParticle& particle, double deltaE)
    {
      // particle falls to rest
      if (particle.m_E - deltaE < particle.m_m) {
        particle.m_E        = particle.m_m;
        particle.m_p        = 0.;
        particle.m_pT       = 0.;
        particle.m_beta     = 0.;
        particle.m_gamma    = 1.;
        particle.m_momentum = Acts::Vector3D(0., 0., 0.);
        particle.m_alive    = false;
      }
      // updatet the parameters
      particle.m_E -= deltaE;
      particle.m_p = std::sqrt(particle.m_E * particle.m_E
                               - particle.m_m * particle.m_m);
      particle.m_momentum = particle.m_p * particle.m_momentum.unit();
      particle.m_pT       = particle.m_momentum.perp();
      particle.m_beta     = (particle.m_p / particle.m_E);
      particle.m_gamma    = (particle.m_E / particle.m_m);
    }

    /// @brief Update the particle with a new position and momentum,
    /// this corresponds to a step update
    ///
    /// @param [in, out] particle Particle that is manipulated
    /// @param [in] position New position after update
    /// @param [in] momentum New momentum after update
    /// @param [in] deltaPathX0 passed since last step
    /// @param [in] deltaPathL0 passed since last step
    /// @param [in] deltaTime The time elapsed
    ///
    /// @return break condition
    bool
    update(SimParticle&          particle,
           const Acts::Vector3D& position,
           const Acts::Vector3D& momentum,
           double                deltaPathX0 = 0.,
           double                deltaPathL0 = 0.,
           double                deltaTime   = 0.)
    {
      particle.m_position = position;
      particle.m_momentum = momentum;
      particle.m_p        = momentum.mag();
      if (particle.m_p) {
        particle.m_pT = momentum.perp();
        particle.m_E  = std::sqrt(particle.m_p * particle.m_p
                                 + particle.m_m * particle.m_m);
        particle.m_timeStamp += deltaTime;
        particle.m_beta  = (particle.m_p / particle.m_E);
        particle.m_gamma = (particle.m_E / particle.m_m);

        // set parameters and check limits
        particle.m_pathInX0 += deltaPathX0;
        particle.m_pathInL0 += deltaPathL0;
        particle.m_timeStamp += deltaTime;
        if (particle.m_pathInX0 >= particle.m_limitInX0
            || particle.m_pathInL0 >= particle.m_limitInL0
            || particle.m_timeStamp > particle.m_timeLimit) {
          particle.m_alive = false;
        }
      }
      return !particle.m_alive;
    }

    /// @brief Boost the particle
    /// Source:
    /// http://www.apc.univ-paris7.fr/~franco/g4doxy4.10/html/_lorentz_vector_8cc_source.html
    /// - boost(double bx, double by, double bz)
    ///
    /// @param [in, out] particle Particle that is manipulated
    /// @param [in] boostVector Direction and value of the boost
    void
    boost(SimParticle& particle, const Acts::Vector3D& boostVector)
    {
      double b2     = boostVector.squaredNorm();
      double ggamma = 1.0 / std::sqrt(1.0 - b2);
      double bp     = boostVector.x() * particle.m_momentum.x()
          + boostVector.y() * particle.m_momentum.y()
          + boostVector.z() * particle.m_momentum.z();
      double gamma2 = b2 > 0 ? (ggamma - 1.0) / b2 : 0.0;

      particle.m_momentum
          = {particle.m_momentum.x() + gamma2 * bp * boostVector.x()
                 + ggamma * boostVector.x() * particle.m_E,
             particle.m_momentum.y() + gamma2 * bp * boostVector.y()
                 + ggamma * boostVector.y() * particle.m_E,
             particle.m_momentum.z() + gamma2 * bp * boostVector.z()
                 + ggamma * boostVector.z() * particle.m_E};
      particle.m_E = ggamma * (particle.m_E + bp);
    }
  };

}  // end of namespace Data
}  // end of namespace FW
