// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <cmath>
#include <stdexcept>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Fatras/Kernel/Particle.hpp"

FW::ParticleGun::ParticleGun(const Config&                       cfg,
                             std::unique_ptr<const Acts::Logger> lgr)
  : m_cfg(cfg), m_logger(std::move(lgr))
{
  // Check that all mandatory configuration parameters are present
  if (m_cfg.evgenCollection.empty()) {
    throw std::invalid_argument("Missing output collection");
  } else if (!m_cfg.randomNumberSvc) {
    throw std::invalid_argument("Missing random numbers service");
  } else if (!m_cfg.barcodeSvc) {
    throw std::invalid_argument("Missing barcode service");
  }

  // Print chosen configuration
  ACTS_DEBUG("Particle gun settings: ");
  ACTS_VERBOSE("- d0  range: " << m_cfg.d0Range[0] << ", " << m_cfg.d0Range[1]);
  ACTS_VERBOSE("- z0  range: " << m_cfg.z0Range[0] << ", " << m_cfg.z0Range[1]);
  ACTS_VERBOSE("- phi range: " << m_cfg.phiRange[0] << ", "
                               << m_cfg.phiRange[1]);
  ACTS_VERBOSE("- eta range: " << m_cfg.etaRange[0] << ", "
                               << m_cfg.etaRange[1]);
  ACTS_VERBOSE("- pt  range: " << m_cfg.ptRange[0] << ", " << m_cfg.ptRange[1]);
}

std::string
FW::ParticleGun::name() const
{
  return "EvgenReader";
}

FW::ProcessCode
FW::ParticleGun::skip(size_t nEvents)
{
  // there is a hard scatter evgen reader
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::ParticleGun::read(AlgorithmContext ctx)
{

  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);
  // what's written out
  RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(ctx);

  UniformDist d0Dist(m_cfg.d0Range.at(0), m_cfg.d0Range.at(1));
  UniformDist z0Dist(m_cfg.z0Range.at(0), m_cfg.z0Range.at(1));
  UniformDist phiDist(m_cfg.phiRange.at(0), m_cfg.phiRange.at(1));
  UniformDist etaDist(m_cfg.etaRange.at(0), m_cfg.etaRange.at(1));
  UniformDist ptDist(m_cfg.ptRange.at(0), m_cfg.ptRange.at(1));
  UniformDist chargeDist(0., 1.);

  std::vector<Fatras::Vertex> vertices;

  // the particles
  std::vector<Fatras::Particle> particles;
  for (size_t ip = 0; ip < m_cfg.nParticles; ip++) {
    // generate random parameters
    double d0  = d0Dist(rng);
    double z0  = z0Dist(rng);
    double phi = phiDist(rng);
    double eta = etaDist(rng);
    double pt  = ptDist(rng);
    // auto   bc  = m_cfg.barcodeSvc->generate(ip);
    // create vertex from random parameters
    Acts::Vector3D vertex(d0 * std::sin(phi), d0 * -std::cos(phi), z0);

    // create momentum from random parameters
    Acts::Vector3D momentum(
        pt * std::cos(phi), pt * std::sin(phi), pt * std::sinh(eta));
    // flip charge and PID if asked for
    int flip = (!m_cfg.randomCharge || chargeDist(rng) < 0.5) ? 1 : -1;
    // the particle should be ready now
    particles.emplace_back(vertex,
                           momentum,
                           m_cfg.mass,
                           flip * m_cfg.charge,
                           flip * m_cfg.pID,
                           ip);
  }
  ACTS_DEBUG("Generated 1 vertex with " << particles.size() << " particles.");
  // the vertices
  vertices.push_back(
      Fatras::Vertex(Acts::Vector3D(0., 0., 0.), {}, std::move(particles)));

  if (ctx.eventStore.add(m_cfg.evgenCollection, std::move(vertices))
      != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
