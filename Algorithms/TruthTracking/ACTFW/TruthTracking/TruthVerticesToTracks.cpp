// This file is part of the Acts project.
//
// Copyright (C) 2019-2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/TruthTracking/TruthVerticesToTracks.hpp"

#include <iostream>
#include <optional>
#include <stdexcept>

#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Utilities/Helpers.hpp"

FW::TruthVerticesToTracksAlgorithm::TruthVerticesToTracksAlgorithm(
    const FW::TruthVerticesToTracksAlgorithm::Config& cfg,
    Acts::Logging::Level                              level)
  : FW::BareAlgorithm("TruthVerticesToTracksAlgorithm", level), m_cfg(cfg)
{
  if (m_cfg.inputParticles.empty()) {
    throw std::invalid_argument("Missing input truth particles collection");
  }
  if (m_cfg.inputPerigees.empty()) {
    throw std::invalid_argument("Missing input perigee collection");
  }
  if (m_cfg.output.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
}

FW::ProcessCode
FW::TruthVerticesToTracksAlgorithm::execute(const AlgorithmContext& ctx) const
{

  // const auto& vertices
  //    = ctx.eventStore.get<std::vector<FW::Data::SimVertex>>(m_cfg.input);

  const auto& particles
      = ctx.eventStore.get<SimParticles>(m_cfg.inputParticles);

  const auto& perigees = ctx.eventStore.get<std::vector<Acts::BoundParameters>>(
      m_cfg.inputPerigees);

  // assert(particles.size()==perigees.size());

  // Vector to store VertexAndTracks extracted from event
  std::vector<VertexAndTracks> vertexAndTracksCollection;

  for (unsigned int i = 0; i < particles.size(); i++) {
    const auto& particle = particles.nth(i);
    const auto& perigee  = perigees[i];

    std::cout << particle->barcode() << std::endl;
    std::cout << perigee.parameters() << std::endl;
  }

  // VertexAndTracks objects to the EventStore
  ctx.eventStore.add(m_cfg.output, std::move(vertexAndTracksCollection));

  return FW::ProcessCode::SUCCESS;
}
