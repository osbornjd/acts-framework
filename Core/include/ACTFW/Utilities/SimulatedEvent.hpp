// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#ifdef USE_HEPMC3
#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"
#endif

namespace FW {

/// @struct SimulatedEvent
///
/// This structure is the default structure for events from external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class E>
struct SimulatedEvent
{
};
}  // FW
