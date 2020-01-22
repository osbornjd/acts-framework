// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "PrintParticles.hpp"

#include <vector>

#include <Acts/Utilities/Logger.hpp>

#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

FW::PrintParticles::PrintParticles(const Config& cfg, Acts::Logging::Level lvl)
  : BareAlgorithm("PrintParticles", lvl), m_cfg(cfg)
{
}

FW::ProcessCode
FW::PrintParticles::execute(const FW::AlgorithmContext& ctx) const
{
  using Event = std::vector<Data::SimVertex>;

  const auto& event = ctx.eventStore.get<Event>(m_cfg.inputEvent);

  ACTS_INFO("event=" << ctx.eventNumber);

  for (size_t ivtx = 0; ivtx < event.size(); ++ivtx) {
    const auto& vtx = event[ivtx];

    ACTS_INFO("vertex idx=" << ivtx);

    for (const auto& prt : vtx.outgoing) {
      ACTS_INFO("  barcode=" << prt.barcode() << " pdg=" << prt.pdg());
    }
  }
  return ProcessCode::SUCCESS;
}
