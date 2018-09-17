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
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"

namespace FW {

/// Typedef the pdg code
typedef int pdg_type;

namespace Data {

  /// @brief Particle information struct for physics process samplers:
  /// - all quatities are calculated at first construction as they may
  ///   be used by downstream samplers
  ///
  /// @note if a sampler changes one of the parameters, consistency
  /// can be broken, so it should update the rest (no checking done)
  class SimParticle
  {
	
	friend class SimParticleModifier;

  public:
    /// @brief Default Constructor
    SimParticle() = default;

    /// @brief Construct a particle consistently
    ///
    /// @param [in] positionThe particle position at construction
    /// @param [in] momentum The particle momentum at construction
    /// @param [in] m The particle mass
    /// @param [in] q The partilce charge
    /// @param [in] pdg PDG code of the particle
    /// @param [in] barcode The particle barcode
    /// @param [in] tStamp is the current time stamp
    SimParticle(const Acts::Vector3D& position,
                const Acts::Vector3D& momentum,
                double                m,
                double                q,
                pdg_type              pdg     = 0,
                barcode_type          barcode = 0,
                double                tStamp  = 0.)
      : m_position(position)
      , m_momentum(momentum)
      , m_m(m)
      , m_q(q)
      , m_p(momentum.norm())
      , m_pT(Acts::VectorHelpers::perp(momentum))
      , m_pdg(pdg)
      , m_barcode(barcode)
      , m_timeStamp(tStamp)
    {
      m_E     = std::sqrt(m_p * m_p + m_m * m_m);
      m_beta  = (m_p / m_E);
      m_gamma = (m_E / m_m);
    }

    /// Default
    SimParticle(const SimParticle& sp) = default;

    /// @brief Access methods: position
    ///
    /// @return Position of the particle
    const Acts::Vector3D&
    position() const
    {
      return m_position;
    }

    /// @brief Access methods: momentum
    ///
    /// @return Momentum of the particle
    const Acts::Vector3D&
    momentum() const
    {
      return m_momentum;
    }

    /// @brief Access methods: p
    ///
    /// @return Magnitude of the momentum of the particle
    const double
    p() const
    {
      return m_p;
    }

    /// @brief Access methods: pT
    ///
    /// @return Transverse momentum of the particle
    const double
    pT() const
    {
      return m_pT;
    }

    /// @brief Access methods: E
    ///
    /// @return Energy of the particle
    const double
    E() const
    {
      return m_E;
    }

    /// @brief Access methods: m
    ///
    /// @return Mass of the particle
    const double
    m() const
    {
      return m_m;
    }

    /// @brief Access methods: beta
    ///
    /// @return Beta value of the particle
    const double
    beta() const
    {
      return m_beta;
    }

    /// @brief Access methods: gamma
    ///
    /// @return Gamma value of the particle
    const double
    gamma() const
    {
      return m_gamma;
    }

    /// @brief Access methods: charge
    ///
    /// @return Charge of the particle
    const double
    q() const
    {
      return m_q;
    }

    /// @brief Access methods: pdg code
    ///
    /// @return PDG code of the particle
    const pdg_type
    pdg() const
    {
      return m_pdg;
    }

    /// @brief Access methods: barcode
    ///
    /// @return Barcode of the particle
    const barcode_type
    barcode() const
    {
      return m_barcode;
    }

    /// @brief Access methods: path/X0
    ///
    /// @return Passed path in X0 of the particle
    const double
    pathInX0() const
    {
      return m_pathInX0;
    }

    /// @brief Access methods: limit/X0
    ///
    /// @return Path limit in X0 of the particle
    const double
    limitInX0() const
    {
      return m_limitInX0;
    }

    /// @brief Access methods: path/L0
    ///
    /// @return Passed path in L0 of the particle
    const double
    pathInL0() const
    {
      return m_limitInX0;
    }

    /// @brief Access methods: limit/L0
    ///
    /// @return Path limit in L0 of the particle
    const double
    limitInL0() const
    {
      return m_limitInL0;
    }
    
    /// @brief Access methods: time stamp
    ///
    /// @return Age of the particle
    const double
    timeStamp() const
    {
      return m_timeStamp;
    }
    
    /// @brief Access methods: limit/time
    ///
    /// @return Maximum age of the particle
    const double
    limitTime() const
    {
      return m_timeLimit;
    }
    
    /// @brief boolean operator indicating the particle to be alive
    operator bool() { return m_alive; }

  private:
    Acts::Vector3D m_position = Acts::Vector3D(0., 0., 0.);  //!< kinematic info
    Acts::Vector3D m_momentum = Acts::Vector3D(0., 0., 0.);  //!< kinematic info

    double       m_m       = 0.;  //!< particle mass
    double       m_E       = 0.;  //!< total energy
    double       m_q       = 0.;  //!< the charge
    double       m_beta    = 0.;  //!< relativistic beta factor
    double       m_gamma   = 1.;  //!< relativistic gamma factor
    double       m_p       = 0.;  //!< momentum magnitude
    double       m_pT      = 0.;  //!< transverse momentum magnitude
    pdg_type     m_pdg     = 0;   //!< pdg code of the particle
    barcode_type m_barcode = 0;   //!< barcode of the particle

    double m_pathInX0 = 0.;  //!< passed path in X0
    double m_limitInX0
        = std::numeric_limits<double>::max();  //!< path limit in X0

    double m_pathInL0 = 0.;  //!< passed path in L0
    double m_limitInL0
        = std::numeric_limits<double>::max();  //!< path limit in X0

    double m_timeStamp = 0.;  //!< passed time elapsed
    double m_timeLimit = std::numeric_limits<double>::max();  // time limit

    bool m_alive = true;  //!< the particle is alive
  };

}  // end of namespace Data
}  // end of namespace FW
