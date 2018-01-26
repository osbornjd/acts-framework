// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"

#include <iostream>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

FW::ReadEvgenAlgorithm::ReadEvgenAlgorithm(
    const Config&                       cfg,
    std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
  if (m_cfg.evgenCollection.empty()) {
    throw std::invalid_argument("Missing output collection");
  } else if (!m_cfg.barcodeSvc) {
    throw std::invalid_argument("Missing barcode service");
  } else if (!m_cfg.randomNumbers) {
    throw std::invalid_argument("Missing random numbers service");
  }
}

std::string
FW::ReadEvgenAlgorithm::name() const
{
  return "ReadEvgenAlgorithm";
}

FW::ProcessCode
FW::ReadEvgenAlgorithm::skip(size_t nEvents)
{
  // there is a hard scatter evgen reader
  std::vector<Acts::ProcessVertex> skipParticles;
  if (m_cfg.hardscatterEventReader
      && m_cfg.hardscatterEventReader->read(skipParticles, nEvents)
          == FW::ProcessCode::ABORT) {
    // error and abort
    ACTS_ERROR("Could not skip " << nEvents << ". Aborting.");
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::ReadEvgenAlgorithm::read(FW::AlgorithmContext ctx)
{
  ACTS_DEBUG("Reading in genertated event info for event no. "
             << ctx.eventNumber);

  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);

  // Setup random number distributions for some quantities
  FW::PoissonDist pileupDist(m_cfg.pileupPoissonParameter);
  FW::GaussDist   vertexTDist(m_cfg.vertexTParameters[0],
                            m_cfg.vertexTParameters[1]);
  FW::GaussDist vertexZDist(m_cfg.vertexZParameters[0],
                            m_cfg.vertexZParameters[1]);

  // prepare the output collection
  std::vector<Acts::ProcessVertex> evgen;

  // get the hard scatter if you have it
  std::vector<Acts::ProcessVertex> hardscatterEvent;
  if (m_cfg.hardscatterEventReader
      && m_cfg.hardscatterEventReader->read(hardscatterEvent, 0, &ctx)
          == FW::ProcessCode::ABORT) {
    ACTS_ERROR("Could not read hard scatter event. Aborting.");
    return FW::ProcessCode::ABORT;
  }

  // generate the number of pileup events
  size_t nPileUpEvents = m_cfg.randomNumbers ? size_t(pileupDist(rng)) : 0;

  ACTS_VERBOSE("- [PU X] number of in-time pileup events : " << nPileUpEvents);

  // reserve quite a lot of space
  double vertexX = vertexTDist(rng);
  double vertexY = vertexTDist(rng);
  double vertexZ = vertexZDist(rng);

  Acts::Vector3D vertex(vertexX, vertexY, vertexZ);

  // fill in the particles
  barcode_type pCounter = 1;
  for (auto& hsVertex : hardscatterEvent) {
    // shift the vertex
    hsVertex.shift(vertex);
    // assign barcodes
    for (auto& oparticle : hsVertex.outgoingParticles()) {
      // generate the new barcode, and assign it
      Acts::ParticleProperties* hsp
          = const_cast<Acts::ParticleProperties*>(&oparticle);
      hsp->assign(m_cfg.barcodeSvc->generate(1, pCounter++));
    }
    // store the hard scatter vertices
    evgen.push_back(hsVertex);
  }

  // loop over the pile-up vertices
  for (size_t ipue = 0; ipue < nPileUpEvents; ++ipue) {
    // reserve quite a lot of space
    double puVertexX = vertexTDist(rng);
    double puVertexY = vertexTDist(rng);
    double puVertexZ = vertexZDist(rng);
    // create the pileup vertex
    vertex = Acts::Vector3D(puVertexX, puVertexY, puVertexZ);
    // get the vertices per pileup event
    std::vector<Acts::ProcessVertex> pileupEvent;
    if (m_cfg.pileupEventReader
        && m_cfg.pileupEventReader->read(pileupEvent, 0, &ctx)
            == FW::ProcessCode::ABORT) {
      ACTS_ERROR("Could not read pile up event " << ipue << ". Aborting.");
      return FW::ProcessCode::ABORT;
    }
    pCounter = 1;
    // loop over pileup vertex per event
    for (auto& puVertex : pileupEvent) {
      // shift to the pile-up vertex
      puVertex.shift(vertex);
      // assign barcodes
      for (auto& oparticle : puVertex.outgoingParticles()) {
        Acts::ParticleProperties* hsp
            = const_cast<Acts::ParticleProperties*>(&oparticle);
        hsp->assign(m_cfg.barcodeSvc->generate(ipue + 2, pCounter++));
      }
      evgen.push_back(puVertex);
    }
  }

  // write to the EventStore
  if (ctx.eventStore.add(m_cfg.evgenCollection, std::move(evgen))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  return FW::ProcessCode::SUCCESS;
}
