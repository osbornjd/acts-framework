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

template <typename simulator_t>
FW::ProcessCode
FW::FatrasAlgorithm<simulator_t>::execute(const AlgorithmContext& ctx) const
{
  using SimEvent = std::vector<FW::Data::SimVertex>;

  // Create an algorithm local random number generator
  RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(ctx);

  // Read Particles from input collection
  const auto& inputEvent
      = ctx.eventStore.get<SimEvent>(m_cfg.inputEventCollection);
  // Output: simulated particles attached to their process vertices
  // we start with a copy of the current event
  SimEvent simulatedEvent(inputEvent);
  // Create the hit collection
  SimHits simulatedHits;

  // The simulation call
  m_cfg.simulator(ctx, rng, simulatedEvent, simulatedHits);

  // Write simulated data to the event store
  // - the simulated particles
  ctx.eventStore.add(m_cfg.simulatedEventCollection, std::move(simulatedEvent));
  // The simulated hits
  ctx.eventStore.add(m_cfg.simulatedHitCollection, std::move(simulatedHits));

  return FW::ProcessCode::SUCCESS;
}
