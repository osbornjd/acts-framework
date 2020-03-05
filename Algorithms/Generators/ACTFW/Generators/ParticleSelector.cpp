// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Generators/ParticleSelector.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "Acts/Utilities/Helpers.hpp"

FW::ParticleSelector::ParticleSelector(const Config&        cfg,
                                       Acts::Logging::Level lvl)
  : FW::BareAlgorithm("ParticleSelector", lvl), m_cfg(cfg)
{
  if (m_cfg.inputEvent.empty()) {
    throw std::invalid_argument("Missing input event collection");
  }
  if (m_cfg.outputEvent.empty()) {
    throw std::invalid_argument("Missing output event collection");
  }
}

FW::ProcessCode
FW::ParticleSelector::execute(const FW::AlgorithmContext& ctx) const
{
  using SimEvent = std::vector<SimVertex>;

  // prepare input/ output types
  const auto& input = ctx.eventStore.get<SimEvent>(m_cfg.inputEvent);
  SimEvent    selected;

  auto within = [](double x, double min, double max) {
    return (min <= x) and (x < max);
  };
  auto isValidParticle = [&](const ActsFatras::Particle& p) {
    auto rho = Acts::VectorHelpers::perp(p.position());
    auto phi = Acts::VectorHelpers::phi(p.unitDirection());
    auto eta = Acts::VectorHelpers::eta(p.unitDirection());
    auto pt  = p.transverseMomentum();
    return within(rho, m_cfg.rhoMin, m_cfg.rhoMax)
        and within(std::abs(p.position().z()), 0, m_cfg.absZMax)
        and within(phi, m_cfg.phiMin, m_cfg.phiMax)
        and within(eta, m_cfg.etaMin, m_cfg.etaMax)
        and within(std::abs(eta), m_cfg.absEtaMin, m_cfg.absEtaMax)
        and within(pt, m_cfg.ptMin, m_cfg.ptMax)
        and (m_cfg.keepNeutral or (p.charge() != 0));
  };

  std::size_t allParticles      = 0;
  std::size_t selectedParticles = 0;

  selected.reserve(input.size());
  for (const auto& inputVertex : input) {
    allParticles += inputVertex.incoming.size();
    allParticles += inputVertex.outgoing.size();

    SimVertex vertex(inputVertex.position4, inputVertex.process);
    // copy selected particles over
    std::copy_if(inputVertex.incoming.begin(),
                 inputVertex.incoming.end(),
                 std::back_inserter(vertex.incoming),
                 isValidParticle);
    std::copy_if(inputVertex.outgoing.begin(),
                 inputVertex.outgoing.end(),
                 std::back_inserter(vertex.outgoing),
                 isValidParticle);

    // only retain vertex if it still contains particles
    if (vertex.incoming.empty() and vertex.outgoing.empty()) { continue; }

    selectedParticles += vertex.incoming.size();
    selectedParticles += vertex.outgoing.size();
    selected.push_back(std::move(vertex));
  }

  ACTS_DEBUG("event " << ctx.eventNumber << " selected " << selectedParticles
                      << " from " << allParticles << " particles");

  ctx.eventStore.add(m_cfg.outputEvent, std::move(selected));
  return ProcessCode::SUCCESS;
}
