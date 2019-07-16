// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "HelloService.hpp"

#include "ACTFW/Framework/WhiteBoard.hpp"

namespace FW {

HelloService::HelloService(const Config& cfg, Acts::Logging::Level level)
  : m_cfg(cfg), m_logger(Acts::getDefaultLogger("HelloService", level))
{
}

std::string
HelloService::name() const
{
  return "HelloService";
}

void
HelloService::startRun()
{
  // nothing to do for this example service.
  // in a real service this is would be a good place to run costly one-time
  // initialization.
}

void
HelloService::prepare(AlgorithmContext& ctx)
{
  // integer division should round down
  std::size_t blockIndex = ctx.eventNumber / m_cfg.eventsPerBlock;
  // add block index to the event store
  ctx.eventStore.add(m_cfg.blockIndexName, std::move(blockIndex));
}

}  // namespace FW
