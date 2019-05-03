// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>

#ifdef USE_HEPMC3
#include "ACTFW/Plugins/HepMC/HepMC3Reader.hpp"
#endif

namespace FW {

/// @struct SimulatedReader
///
/// This structure is the default structure to read runs from external
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <class R, class E>
struct SimulatedReader
{
};
}  // FW
