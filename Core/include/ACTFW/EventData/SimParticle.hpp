// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cfloat>
#include <cmath>

#include <Acts/Utilities/Definitions.hpp>
#include <boost/container/flat_set.hpp>

#include "ACTFW/EventData/Barcode.hpp"

namespace FW {

/// A type that stores PDG particle type ids.
using pdg_type = int;

namespace Data {

  /// Particle state information for simulations.
  class SimParticle
  {
  public:
    SimParticle()                   = default;
    SimParticle(const SimParticle&) = default;
    /// @param position The particle position at construction
    /// @param momentum The particle momentum at construction
    /// @param mass     The particle mass
    /// @param charge   The particle charge
    /// @param pdg      The particle type/ PDG id
    /// @param barcode  The particle barcode/ identifier
    /// @param time     The particle time
    SimParticle(const Acts::Vector3D& position,
                const Acts::Vector3D& momentum,
                double                mass,
                double                charge,
                pdg_type              pdg     = 0,
                barcode_type          barcode = 0,
                double                time    = 0.)
      : m_position(position)
      , m_time(time)
      , m_momentum(momentum)
      , m_mass(mass)
      , m_charge(charge)
      , m_pdg(pdg)
      , m_barcode(barcode)
    {
    }

    /// Set limits for livelihood checks.
    ///
    /// @param x0Limit   The limit in X0 to be passed
    /// @param l0Limit   The limit in L0 to be passed
    /// @param timeLimit The readout time limit to be passed
    void
    setLimits(double x0Limit, double l0Limit, double timeLimit = DBL_MAX)
    {
      m_limitInX0 = x0Limit;
      m_limitInL0 = l0Limit;
      m_limitTime = timeLimit;
    }

    /// Place the particle at the given position and reset its identifier.
    void
    place(Acts::Vector3D position, barcode_type barcode, double time = 0.)
    {
      m_position = position;
      m_time     = time;
      m_barcode  = barcode;
    }
    /// Update the particle momentum to the given value.
    void
    scatter(Acts::Vector3D momentum)
    {
      m_momentum = momentum;
    }
    /// Reduce the particle energy by the given amount.
    void
    energyLoss(double deltaE)
    {
      auto reducedE = E() - deltaE;
      if (m_mass < reducedE) {
        // particle is not at rest after update
        // E² = p² + m² -> p² = E² - m² -> p = sqrt(E² - m²)
        auto reducedP = std::sqrt(reducedE * reducedE - m_mass * m_mass);
        m_momentum.normalize();
        m_momentum *= reducedP;
      } else {
        // particle falls to rest
        m_momentum = Acts::Vector3D(0., 0., 0.);
      }
    }
    /// Update the particle to a new position and momentum.
    ///
    /// @param position    New position after update
    /// @param momentum    New momentum after update
    /// @param deltaPathX0 Passed since last step
    /// @param deltaPathL0 Passed since last step
    /// @param deltaTime   The time elapsed
    /// @returns true if the particle is not alive anymore after the update
    bool
    update(const Acts::Vector3D& position,
           const Acts::Vector3D& momentum,
           double                deltaPathX0 = 0.,
           double                deltaPathL0 = 0.,
           double                deltaTime   = 0.)
    {
      m_position = position;
      m_time = deltaTime;
      m_momentum = momentum;
      m_pathInX0 += deltaPathX0;
      m_pathInL0 += deltaPathL0;
      return !(*this);
    }

    const Acts::Vector3D&
    position() const
    {
      return m_position;
    }
    double
    time() const
    {
      return m_time;
    }

    /// Particle momentum vector.
    const Acts::Vector3D&
    momentum() const
    {
      return m_momentum;
    }
    /// Absolute particle momentum.
    double
    p() const
    {
      return m_momentum.norm();
    }
    /// Absolute transverse particle momentum.
    double
    pT() const
    {
      return std::hypot(m_momentum[Acts::eX], m_momentum[Acts::eY]);
    }
    /// Particle energy.
    double
    E() const
    {
      return std::hypot(p(), m_mass);
    }
    /// Particle mass.
    double
    m() const
    {
      return m_mass;
    }
    /// Particle relativistic velocity.
    double
    beta() const
    {
      return 1.0 / std::hypot(1.0, m_mass / p());
    }
    /// Particle gamma factor.
    double
    gamma() const
    {
      return std::hypot(1.0, p() / m_mass);
    }

    /// Particle charge.
    double
    q() const
    {
      return m_charge;
    }

    /// Particle type/ PDG id.
    pdg_type
    pdg() const
    {
      return m_pdg;
    }
    /// Particle identifier/ barcode.
    barcode_type
    barcode() const
    {
      return m_barcode;
    }

    /// How many radiation lengths of material has this particle passed.
    double
    pathInX0() const
    {
      return m_pathInX0;
    }
    /// How much propagated path length has this particle accumulated.
    double
    pathInL0() const
    {
      return m_limitInX0;
    }
    /// Limit on passed radiation lengths to consider particle alive.
    double
    limitInX0() const
    {
      return m_limitInX0;
    }
    /// Limit on propagated path length to consider particle alive.
    const double
    limitInL0() const
    {
      return m_limitInL0;
    }

    /// Whether the particle is alive, i.e. non-zero momentum and within limits.
    operator bool() const
    {
      return (0 < p()) and (m_time < m_limitTime) and (m_pathInX0 < m_limitInX0)
          and (m_pathInL0 < m_limitInL0);
    }

  private:
    /// TODO replace by combined 4d position
    Acts::Vector3D m_position = Acts::Vector3D(0., 0., 0.);  //!< kinematic info
    double         m_time     = 0.;  //!< passed time elapsed
    Acts::Vector3D m_momentum = Acts::Vector3D(0., 0., 0.);  //!< kinematic info
    double         m_mass     = 0.;                          //!< particle mass
    double         m_charge   = 0.;                          //!< the charge
    pdg_type       m_pdg      = 0;         //!< pdg code of the particle
    barcode_type   m_barcode  = 0;         //!< barcode of the particle
    double         m_pathInX0 = 0.;        //!< passed path in X0
    double         m_pathInL0 = 0.;        //!< passed path in L0
    double         m_limitTime = DBL_MAX;  //!< time limit
    double         m_limitInX0 = DBL_MAX;  //!< path limit in X0
    double         m_limitInL0 = DBL_MAX;  //!< path limit in X0
  };

}  // namespace Data

namespace detail {
  struct CompareSimParticleBarcode
  {
    using is_transparent = void;
    bool
    operator()(const Data::SimParticle& left,
               const Data::SimParticle& right) const
    {
      return left.barcode() < right.barcode();
    }
    bool
    operator()(barcode_type left, const Data::SimParticle& right) const
    {
      return left < right.barcode();
    }
    bool
    operator()(const Data::SimParticle& left, barcode_type right) const
    {
      return left.barcode() < right;
    }
  };
}  // namespace detail

/// A container of particles that can be accessed by particle id/ barcode.
using SimParticles
    = boost::container::flat_set<Data::SimParticle,
                                 detail::CompareSimParticleBarcode>;

}  // end of namespace FW