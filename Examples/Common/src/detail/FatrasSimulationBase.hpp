// This file is part of the Acts project.
//
// Copyright (C) 2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Utilities/OptionsFwd.hpp"

#include <memory>

namespace FW {
class Sequencer;
class RandomNumbers;
}  // namespace FW

namespace Acts {
class TrackingGeometry;
}

/// @brief Simulation setup
///
/// @tparam bfield_t Type of the bfield for the simulation to be set up
///
/// @param fieldMap The field map for the simulation setup
/// @param sequencer The framework sequencer
/// @param vm The boost variable map to resolve
/// @param tGeometry The TrackingGeometry for the tracking setup
/// @param barcodesSvc The barcode service to be used for the simulation
/// @param randomNumberSvc The random number service to be used for the
/// simulation
void
setupSimulation(boost::program_options::variables_map&        vm,
                FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
                std::shared_ptr<FW::RandomNumbers>            randomNumberSvc);
