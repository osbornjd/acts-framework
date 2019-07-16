// This file is part of the Acts project.
//
// Copyright (C) 2017,2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "HelloWorldAlgorithm.hpp"

FW::HelloWorldAlgorithm::HelloWorldAlgorithm(Acts::Logging::Level level)
  : FW::BareAlgorithm("HelloWorld", level)
{
}

FW::ProcessCode
FW::HelloWorldAlgorithm::execute(const AlgorithmContext& context) const
{
  ACTS_INFO(" Hello World! (from event " << context.eventNumber << ")");
  ACTS_DEBUG("  - that's an ACTS_DEBUG message");
  ACTS_VERBOSE("  - that's an ACTS_VERBOSE message");
  return FW::ProcessCode::SUCCESS;
}
