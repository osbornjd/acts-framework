// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <array>
#include "ACTFW/Readers/IReaderT.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Fatras/Kernel/Particle.hpp"

using range = std::array<double, 2>;

namespace FW {

class BarcodeSvc;
class RandomNumbersSvc;

using InputReader = IReaderT<std::vector<Fatras::Vertex>>;  

/// @class ParticleGun
///
/// Particle gun implementation of the IReader. It can be used as
/// particle input for the Fatras example.
///
/// It generates particles of given type with random momentum and random vertex
/// in a given range. It fills a vector of particle properties for feeding into
/// fast simulation.
///
class ParticleGun : public InputReader
{
public:
  struct Config
  {
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
  name() const final override { return "ParticleGun"; }

  // clang-format off
  /// @copydoc FW::IReaderT::read(std::vector<Fatras::Vertex>& pProperties,size_t,const FW::AlgorithmContext*)
  // clang-format on
  FW::ProcessCode
  read(std::vector<Fatras::Vertex>& pProperties,
       size_t                       skip    = 0,
       const FW::AlgorithmContext*  context = nullptr) final override;

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

}  // namespace FW
