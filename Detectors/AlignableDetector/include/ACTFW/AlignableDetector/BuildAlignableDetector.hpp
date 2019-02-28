// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <vector>
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace Acts {
class TrackingGeometry;
}

namespace FW {

namespace Alignable {

  /// Global method to build the generic tracking geometry
  /// @param lvl is the debug logging level
  /// @param version is the detector version
  /// return a unique vector to the tracking geometry
  std::unique_ptr<const Acts::TrackingGeometry>
  buildAlignableDetector(Acts::Logging::Level surfaceLLevel
                         = Acts::Logging::INFO,
                         Acts::Logging::Level layerLLevel = Acts::Logging::INFO,
                         Acts::Logging::Level volumeLLevel
                         = Acts::Logging::INFO,
                         size_t version = 0);

}  // namespace Alignable

}  // namespace FW
