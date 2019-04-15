// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <set>
#include <string>
#include "Acts/Detector/TrackingVolume.hpp"

class GeoVPhysVol;

namespace FW {

class GeoModelPixel
{
public:

	Acts::MutableTrackingVolumePtr
	buildPixel(GeoVPhysVol const* bp) const;

private:

	std::vector<GeoVPhysVol const*>
	findLayers(GeoVPhysVol const* vol) const;

	std::set<std::string> layerKeys = {"Layer0", "Layer1", "Layer2", "Layer3"};
};
}  // namespace FW