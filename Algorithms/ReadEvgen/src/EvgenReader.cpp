// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/EvgenReader.hpp"

FW::EvgenReader::EvgenReader(const Config&                       cfg,
                             std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
  if (m_cfg.evgenCollection.empty()) {
    throw std::invalid_argument("Missing output collection");
  } else if (!m_cfg.barcodeSvc) {
    throw std::invalid_argument("Missing barcode service");
  } else if (!m_cfg.randomNumberSvc) {
    throw std::invalid_argument("Missing random numbers service");
  }
}

std::string
FW::EvgenReader::name() const
{
  return "EvgenReader";
}

FW::ProcessCode
FW::EvgenReader::skip(size_t nEvents)
{
  // there is a hard scatter evgen reader
  std::vector<Data::SimVertex<>> skipEvents;
  if (m_cfg.hardscatterEventReader
      && m_cfg.hardscatterEventReader->read(skipEvents, nEvents)
          == FW::ProcessCode::ABORT) {
    // error and abort
    ACTS_ERROR("Could not skip " << nEvents << ". Aborting.");
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::EvgenReader::read(FW::AlgorithmContext ctx)
{
  ACTS_DEBUG("Reading in genertated event info for event no. "
             << ctx.eventNumber);

  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(ctx);

  // Setup random number distributions for some quantities
  FW::PoissonDist pileupDist(m_cfg.pileupPoissonParameter);
  FW::GaussDist   vertexTDist(m_cfg.vertexTParameters[0],
                            m_cfg.vertexTParameters[1]);
  FW::GaussDist vertexZDist(m_cfg.vertexZParameters[0],
                            m_cfg.vertexZParameters[1]);

  // prepare the output collection
  std::vector<Data::SimVertex<>> evgen;

  // get the hard scatter if you have it
  std::vector<Data::SimVertex<>> hardscatterEvent;
  // Always provide the context to the hard scatter Event
  const AlgorithmContext* contextPtr = &ctx;
  if (m_cfg.hardscatterEventReader
      && m_cfg.hardscatterEventReader->read(hardscatterEvent, 0, &ctx)
          == FW::ProcessCode::ABORT) {
    ACTS_ERROR("Could not read hard scatter event. Aborting.");
    return FW::ProcessCode::ABORT;
  }

  // generate the number of pileup events
  size_t nPileUpEvents = m_cfg.randomNumberSvc ? size_t(pileupDist(rng)) : 0;
  ACTS_VERBOSE("Number of in-time pileup events : " << nPileUpEvents);

  // reserve the number of pileup events + one hard scatter event
  evgen.reserve(nPileUpEvents + 1);

  // the event counter
  barcode_type eCounter = 0;
  // the particle counter
  barcode_type pCounter = 0;

  // lambda for vertex processing
  auto processVertex
      = [&evgen, &pCounter, &eCounter](const Acts::Vector3D& shift,
                                       Data::SimVertex<>& vertex,
                                       BarcodeSvc&        barcodeSvc) -> void {
    // shift the vertex
    vertex.position = vertex.position + shift;
    // shift and assign barcodes to outgoing particles
    for (auto& op : vertex.out) {
      // shift the particle position by the smeared vertex & set barcode
      op.place(op.position() + shift,
               barcodeSvc.generate(eCounter, pCounter++));
    }
    // store the hard scatter vertices
    evgen.push_back(vertex);
  };

  // create a vertex distribution
  double         vertexX = vertexTDist(rng);
  double         vertexY = vertexTDist(rng);
  double         vertexZ = vertexZDist(rng);
  Acts::Vector3D vertexShift(vertexX, vertexY, vertexZ);

  // hard scatter section
  for (auto& hsVertex : hardscatterEvent) {
    // create the new vertex position
    processVertex(vertexShift, hsVertex, *m_cfg.barcodeSvc);
    ++eCounter;
  }

  // pile-up section
  for (size_t ipue = 0; ipue < nPileUpEvents; ++ipue) {
    // reserve quite a lot of space
    double puVertexX = vertexTDist(rng);
    double puVertexY = vertexTDist(rng);
    double puVertexZ = vertexZDist(rng);
    // create the pileup vertex
    vertexShift = Acts::Vector3D(puVertexX, puVertexY, puVertexZ);
    // Get the vertices per pileup event
    // only provide the Context for the initial call to set the seed
    contextPtr = ipue ? nullptr : &ctx;
    std::vector<Data::SimVertex<>> pileupEvent;
    if (m_cfg.pileupEventReader
        && m_cfg.pileupEventReader->read(pileupEvent, 0, contextPtr)
            == FW::ProcessCode::ABORT) {
      ACTS_ERROR("Could not read pile up event " << ipue << ". Aborting.");
      return FW::ProcessCode::ABORT;
    }
    pCounter = 0;
    // loop over pileup vertex per event
    for (auto& puVertex : pileupEvent) {
      // create the new vertex position
      processVertex(vertexShift, puVertex, *m_cfg.barcodeSvc);
      ++eCounter;
    }
  }

  // shuffle the HS event
  if (m_cfg.shuffleEvents) std::random_shuffle(evgen.begin(), evgen.end());

  // write to the EventStore
  if (ctx.eventStore.add(m_cfg.evgenCollection, std::move(evgen))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}
