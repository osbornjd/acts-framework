// This file is part of the Acts project.
//
// Copyright (C) 2019  Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>

#include "ACTFW/AlignableDetector/AlignableDetectorElement.hpp"
#include "ACTFW/GenericDetector/LayerBuilderT.hpp"

namespace FW {
namespace Alignable {
  using AlignableLayerBuilder
      = Generic::LayerBuilderT<AlignableDetectorElement>;

}  // end of namespace Generic
}  // end of namespace FW
