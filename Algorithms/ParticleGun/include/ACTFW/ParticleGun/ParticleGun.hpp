// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <array>
#include "ACTFW/Framework/IReader.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"

using range = std::array<double, 2>;

namespace FW {

class BarcodeSvc;
class RandomNumbersSvc;

/// @class ParticleGun
///
/// Particle gun implementation of the IReader. It can be used as
/// particle input for the Fatras example.
///
/// It generates particles of given type with random momentum and random vertex
/// in a given range. It fills a vector of particle properties for feeding into
/// fast simulation.
///
class ParticleGun : public FW::IReader
{
public:
  struct Config
  {
    /// Indicate whether to use the gun or not
    bool on = true;
    /// output collection for generated particles
    std::string evgenCollection = "EvgenParticles";
    /// number of events
    size_t nEvents = 1;
    /// number of particles
    size_t nParticles = 1;
    /// low, high for d0 range
    range d0Range = {{0., 1 * Acts::units::_mm}};
    /// low, high for z0 range
    range z0Range = {{-100 * Acts::units::_mm, 100 * Acts::units::_mm}};
    /// low, high for phi range
    range phiRange = {{-M_PI, M_PI}};
    /// low, high for eta range
    range etaRange = {{-3., 3.}};
    /// low, high for pt range
    range ptRange = {{100 * Acts::units::_MeV, 10 * Acts::units::_GeV}};
    /// the mass of the particle
    double mass = 0.;
    /// the charge of the particle
    double charge = 0.;
    /// the pdg type of the particle
    int pID = 0.;
    // randomize the charge (indicates PID flip)
    bool randomCharge = false;
    // FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumberSvc = nullptr;
    std::shared_ptr<FW::BarcodeSvc>       barcodeSvc      = nullptr;
  };

  /// Constructor
  /// @param cfg is the configuration class
  ParticleGun(const Config&                       cfg,
              std::unique_ptr<const Acts::Logger> logger
              = Acts::getDefaultLogger("ParticleGun", Acts::Logging::INFO));

  /// Framework name() method
  std::string
  name() const final override;

  /// Skip a few events in the IO stream
  /// @param [in] nEvents is the number of skipped events
  FW::ProcessCode
  skip(size_t nEvents) final override;

  /// Read out data from the input stream
  FW::ProcessCode
  read(FW::AlgorithmContext ctx) final override;

  /// Return the number of events
  virtual size_t
  numEvents() const final override;

private:
  Config                              m_cfg;
  std::unique_ptr<const Acts::Logger> m_logger;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

/// Return of the number events
inline size_t
ParticleGun::numEvents() const
{
  return m_cfg.nEvents;
}

}  // namespace FW
