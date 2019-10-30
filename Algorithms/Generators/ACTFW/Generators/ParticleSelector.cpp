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

FW::ParticleSelector::ParticleSelector(const Config&        cfg,
                                       Acts::Logging::Level level)
  : FW::BareAlgorithm("Selector", level), m_cfg(cfg)
{
  if (m_cfg.input.empty()) {
    throw std::invalid_argument("Missing input collection");
  }
  if (m_cfg.output.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
}

FW::ProcessCode
FW::ParticleSelector::execute(const FW::AlgorithmContext& ctx) const
{
  std::vector<Data::SimVertex> selected;

  // get input particles
  const auto& input
      = ctx.eventStore.get<std::vector<Data::SimVertex>>(m_cfg.input);

  auto within = [](double x, double min, double max) {
    return (min <= x) and (x < max);
  };
  auto isValidParticle = [&](const Data::SimParticle& p) {
    auto rho = std::hypot(p.position().x(), p.position().y());
    auto phi = std::atan2(p.momentum().y(), p.momentum().x());
    auto eta = std::atanh(p.momentum().z() / p.momentum().norm());
    auto pt  = std::hypot(p.momentum().x(), p.momentum().y());
    return within(rho, 0, m_cfg.rhoMax)
        and within(std::abs(p.position().z()), 0, m_cfg.absZMax)
        and within(phi, m_cfg.phiMin, m_cfg.phiMax)
        and within(eta, m_cfg.etaMin, m_cfg.etaMax)
        and within(std::abs(eta), m_cfg.absEtaMin, m_cfg.absEtaMax)
        and within(pt, m_cfg.ptMin, m_cfg.ptMax)
        and (m_cfg.keepNeutral or (p.q() != 0));
  };

  // copy selected vertices over to new collection
  size_t allParticles = 0;
  size_t selParticles = 0;

  for (const auto& vertex : input) {

    allParticles += vertex.incoming.size();
    allParticles += vertex.outgoing.size();

    Data::SimVertex sel;
    sel.position    = vertex.position;
    sel.time        = vertex.time;
    sel.processCode = vertex.processCode;

    // copy selected particles over
    std::copy_if(vertex.incoming.begin(),
                 vertex.incoming.end(),
                 std::back_inserter(sel.incoming),
                 isValidParticle);
    std::copy_if(vertex.outgoing.begin(),
                 vertex.outgoing.end(),
                 std::back_inserter(sel.outgoing),
                 isValidParticle);

    // only retain vertex if it still contains particles
    if (not sel.incoming.empty() or not sel.outgoing.empty()) {
      selParticles += sel.incoming.size();
      selParticles += sel.outgoing.size();
      selected.push_back(std::move(sel));
    }
  }

  ACTS_DEBUG("event " << ctx.eventNumber << " selected " << selected.size()
                      << " from " << input.size() << " vertices");
  ACTS_DEBUG("event " << ctx.eventNumber << " selected " << selParticles
                      << " from " << allParticles << " particles");

  // write selected particles
  ctx.eventStore.add(m_cfg.output, std::move(selected));

  return ProcessCode::SUCCESS;
}
