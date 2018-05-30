// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EXTRAPOLATIONUTILS
#define ACTFW_EXTRAPOLATIONUTILS

#include <memory>

#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace Acts {
class IExtrapolationEngine;
class TrackingGeometry;
}

namespace FW {

/// Method to setup the Extrapolation Engines
/// @param geo shared_ptr to the Acts::TrackingGeometry which should be used for
/// the extrapolation
/// @return a fully initialized Acts::ExtrapoltionEngine
template <class MagneticField = Acts::ConstantBField>
std::unique_ptr<Acts::IExtrapolationEngine>
initExtrapolator(const std::shared_ptr<const Acts::TrackingGeometry>& geo,
                 std::shared_ptr<MagneticField>                       magField,
                 Acts::Logging::Level eLogLevel);
}

#include "ExtrapolationUtils.ipp"

#endif  // ACTFW_EXTRAPOLATIONUTILS
