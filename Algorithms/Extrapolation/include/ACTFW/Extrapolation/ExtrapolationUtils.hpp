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

#include "ACTS/Extrapolation/IMaterialEffectsEngine.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
class IExtrapolationEngine;
class TrackingGeometry;
}

namespace FW {

/// @param magField The magnetic field to be used
/// @param eLogLevel The logging level of the extrapolation
/// @param matEffectsEngine Possibility to hand over material effects engine
/// (for fatras). If no material effects engine is set the
/// Acts::MaterialEffectsEngine (for reconstruction) will be set as default.
/// @return a fully initialized Acts::ExtrapoltionEngine
template <class MagneticField = Acts::ConstantBField>
std::unique_ptr<Acts::IExtrapolationEngine>
initExtrapolator(
    const std::shared_ptr<const Acts::TrackingGeometry>& geo,
    std::shared_ptr<MagneticField>                       magField,
    Acts::Logging::Level                                 eLogLevel,
    std::shared_ptr<const Acts::IMaterialEffectsEngine>  matEffectsEngine
    = nullptr);
}

#include "ExtrapolationUtils.ipp"

#endif  // ACTFW_EXTRAPOLATIONUTILS
