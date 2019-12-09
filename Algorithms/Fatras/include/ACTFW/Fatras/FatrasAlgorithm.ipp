// This file is part of the Acts project.
//
// Copyright (C) 2018-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

template <typename simulator_t>
FW::FatrasAlgorithm<simulator_t>::FatrasAlgorithm(const Config&        cfg,
                                                  Acts::Logging::Level lvl)
  : FW::BareAlgorithm("FatrasAlgorithm", lvl), m_cfg(cfg)
{
}

namespace FW {
namespace detail {

  // helper struct to store simulation hits without maintaining geometry order.
  struct UnorderedSimHits
  {
    SimHits::sequence_type hits;

    void
    insert(const Data::SimHit& hit)
    {
      hits.push_back(hit);
    }
  };

}  // namespace detail
}  // namespace FW

template <typename simulator_t>
FW::ProcessCode
FW::FatrasAlgorithm<simulator_t>::execute(const AlgorithmContext& ctx) const
{
  using SimEvent = std::vector<FW::Data::SimVertex>;

  // read input containers
  const auto& inputEvent
      = ctx.eventStore.get<SimEvent>(m_cfg.inputEventCollection);

  // prepare output containers
  // the event is modified in-place; initialize with a copy
  SimEvent simulatedEvent(inputEvent);
  // simulated hits are stored in a geometry-ordered flat container. to avoid
  // large performance impact from maintaining this order during the simulation
  // the hits are stored first in the order in which they are created.
  SimHits                  simulatedHits;
  detail::UnorderedSimHits unorderedHits;

  // run the simulation w/ a local random generator
  auto rng = m_cfg.randomNumberSvc->spawnGenerator(ctx);
  m_cfg.simulator(ctx, rng, simulatedEvent, unorderedHits);

  // restablish geometry ordering for the output hits container
  simulatedHits.adopt_sequence(std::move(unorderedHits.hits));

  // write data to the event store
  ctx.eventStore.add(m_cfg.simulatedEventCollection, std::move(simulatedEvent));
  ctx.eventStore.add(m_cfg.simulatedHitCollection, std::move(simulatedHits));
  return FW::ProcessCode::SUCCESS;
}
