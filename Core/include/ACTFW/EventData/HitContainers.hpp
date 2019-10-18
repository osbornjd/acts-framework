// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>

#include <boost/container/flat_map.hpp>

#include "ACTFW/EventData/Barcode.hpp"

namespace FW {

/// Event-unique hit id.
using HitId = uint64_t;

/// Map hit identifiers to truth particle id/ barcode.
///
/// A hit can have multiple underlying truth particles. A multimap can contain
/// multiple values for the same key and we can find all particles for a given
/// hit easily using `.equal_range`.
using HitParticleMap = boost::container::flat_multimap<HitId, barcode_type>;

}  // namespace FW
