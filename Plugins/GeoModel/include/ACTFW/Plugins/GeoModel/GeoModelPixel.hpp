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

namespace Acts
{
	class SurfaceArray;
}

namespace FW {

/// @class This class translates the pixel detector of a GeoModel geometry into an Acts::TrackingVolume
class GeoModelPixel
{
public:

	/// @brief This function builds the pixel detector as Acts::TrackingVolume from a given GeoModel volume
	///
	/// @param [in] pd The pixel detector volume
	///
	/// @return The Acts volume
	Acts::MutableTrackingVolumePtr
	buildPixel(GeoVPhysVol const* pd) const;

private:

std::unique_ptr<Acts::SurfaceArray>
surfaceArray(GeoVPhysVol const* lay) const;

	/// @brief This function takes a GeoModel volume and builds its contained layers in Acts types
	///
	/// @param [in] vol The GeoModel volume
	///
	/// @return Vector of Acts layers
	std::vector<std::shared_ptr<const Acts::Layer>>
	buildLayers(GeoVPhysVol const* vol) const;

	/// @brief This function builds the Acts layer array of a given GeoModel volume
	///
	/// @param [in] vol GeoModel volume
	///
	/// @return Acts layer array
	std::unique_ptr<const Acts::LayerArray>
	buildLayerArray(GeoVPhysVol const* vol) const;
	
	/// @brief This function builds the pixel detector barrel as Acts::TrackingVolume from a given GeoModel volume
	///
	/// @param [in] pdBarrel The pixel detector barrel volume
	///
	/// @return The Acts volume
	std::shared_ptr<Acts::TrackingVolume>
	buildPixelBarrel(GeoVPhysVol const* pdBarrel) const;

	/// @brief This function builds the pixel detector EndCap as Acts::TrackingVolume from a given GeoModel volume
	///
	/// @param [in] pdEndCap The pixel detector EndCap volume
	///
	/// @return The Acts volume
	std::shared_ptr<Acts::TrackingVolume>
	buildPixelEndCap(GeoVPhysVol const* pdEndCap) const;

	/// Set of keys we search for in the GeoModel to find the right volumes in the tree
	std::set<std::string> m_volumeKeys = {"Barrel", "EndCap"};
	/// Set of keys we search for int the GeoModel to find the right layers in the volumes
	std::set<std::string> m_layerKeys = {"Layer0", "Layer1", "Layer2", "Layer3"};
};
}  // namespace FW