// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "Acts/Utilities/GeometryID.hpp"

template <typename simulator_t, typename event_collection_t, typename hit_t>
FW::FatrasAlgorithm<simulator_t, event_collection_t, hit_t>::FatrasAlgorithm(
    const Config&        cfg,
    Acts::Logging::Level loglevel)
  : FW::BareAlgorithm("FatrasAlgorithm", loglevel), m_cfg(cfg)
{
}

template <typename simulator_t, typename event_collection_t, typename hit_t>
FW::ProcessCode
FW::FatrasAlgorithm<simulator_t, event_collection_t, hit_t>::execute(
    const FW::AlgorithmContext context) const
{

  // Create an algorithm local random number generator
  RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(context);

  // read Particles from input collection
  const event_collection_t* inputEvent = nullptr;
  if (context.eventStore.get(m_cfg.inputEventCollection, inputEvent)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  ACTS_DEBUG("Read collection '" << m_cfg.inputEventCollection << "' with "
                                 << inputEvent->size()
                                 << " vertices");

  // output: simulated particles attached to their process vertices
  // we start with a copy of the current event
  event_collection_t simulatedEvent(*inputEvent);

  // nested hit collection struct to shield fatras from FW data structures
  struct HitCollection
  {
    /// The actual hit collection
    FW::DetectorData<geo_id_value, hit_t> hits;

    /// The hit inserter method
    void
    insert(hit_t hit)
    {
      /// decode the geometry ID values
      auto         geoID    = hit.surface->geoID();
      geo_id_value volumeID = geoID.value(Acts::GeometryID::volume_mask);
      geo_id_value layerID  = geoID.value(Acts::GeometryID::layer_mask);
      geo_id_value moduleID = geoID.value(Acts::GeometryID::sensitive_mask);
      /// insert the simulate hit into the collection
      FW::Data::insert(hits, volumeID, layerID, moduleID, std::move(hit));
    }
  };

  // create the hit collection
  HitCollection simulatedHits;

  // the simulation call
  m_cfg.simulator(rng, simulatedEvent, simulatedHits);

  // write simulated data to the event store
  // - the simulated particles
  if (context.eventStore.add(m_cfg.simulatedEventCollection,
                             std::move(simulatedEvent))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  // - the soimulated hits
  if (context.eventStore.add(m_cfg.simulatedHitCollection,
                             std::move(simulatedHits.hits))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}
