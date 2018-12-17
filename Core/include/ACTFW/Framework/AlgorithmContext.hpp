// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

namespace FW {

class WhiteBoard;

/// Aggregated information to run one algorithm over one event.
struct AlgorithmContext
{
  size_t      algorithmNumber;  ///< Unique algorithm identifier
  size_t      eventNumber;      ///< Unique event identifier
  WhiteBoard& eventStore;       ///< Per-event data store
};

}  // namespace FW
