// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_EVENTDATA_SIMHIT_H
#define ACTFW_EVENTDATA_SIMHIT_H

#include <map>
#include <vector>
#include "ACTFW/Barcode/Barcode.hpp"
#include "Acts/Utilities/Definitions.hpp"

namespace Acts {
class Surface;
}  // namespace Acts

namespace FW {

/// SimHit struct to be used with the ACTS framework
struct SimHit
{
  const Acts::Surface* surface    = nullptr;
  Acts::Vector3D       position   = Acts::Vector3D{0, 0, 0.};
  Acts::Vector3D       momentum   = Acts::Vector3D{0, 0, 0.};
  double               pathLength = 0.;
  double               value      = 0.;
  barcode_type         barcode    = 0;
};

}  // namespace FW

#endif
