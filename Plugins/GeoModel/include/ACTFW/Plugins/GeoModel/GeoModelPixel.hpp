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

namespace Acts {
class SurfaceArray;
}

namespace FW {

/// @class This class translates the pixel detector of a GeoModel geometry into
/// an Acts::TrackingVolume
class GeoModelPixel
{
public:
	/// Type for the material look up. In order to acces the right surface material for the assignment to a surface, the name needs to be parsed. In GeoModel this is structured bottom to top (e.g. /ModuleBrl6/Ladder51/Layer3/Barrel/Pixel). Using this string, each of the elements can be uniquely identified and for each of these string we require a corresponding material assignment.
	using MaterialDict = std::map<std::string, std::shared_ptr<const Acts::SurfaceMaterial>>;

  /// @brief This function builds the pixel detector as Acts::TrackingVolumes
  /// from a given GeoModel volume
  ///
  /// @param [in] pd The pixel detector volume
  /// @param [in] matDict Lookup of the material that is assign to a each surface
  ///
  /// @return The Acts volumes
  std::vector<Acts::MutableTrackingVolumePtr>
  buildPixel(GeoVPhysVol const* pd, const MaterialDict& matDict) const;

private:
  std::unique_ptr<Acts::SurfaceArray>
  surfaceArray(GeoVPhysVol const* lay, const MaterialDict& matDict) const;

  /// @brief This function takes a GeoModel volume and builds its contained
  /// layers in Acts types
  ///
  /// @param [in] vol The GeoModel volume
  /// @param [in] transformationVolume The transformation of the encapsulating
  /// volume - Required since GeoModel stores them only relative to each other
  /// along the tree
  /// @param [in] barrel Boolean flag to indicate if the barrel (true) or endcap
  /// (false) is considered
  /// @param [in] matDict Lookup of the material that is assign to a each surface
  ///
  /// @return Vector of Acts layers
  std::vector<std::shared_ptr<const Acts::Layer>>
  buildLayers(GeoVPhysVol const*                       vol,
              std::shared_ptr<const Acts::Transform3D> transformationVolume,
              bool                                     barrel, const MaterialDict& matDict) const;

  /// @brief This function builds the Acts layer array of a given GeoModel
  /// volume
  ///
  /// @param [in] vol GeoModel volume
  /// @param [in] transformationVolume The transformation of the encapsulating
  /// volume - Required since GeoModel stores them only relative to each other
  /// along the tree
  /// @param [in] barrel Boolean flag to indicate if the barrel (true) or endcap
  /// (false) is considered
  /// @param [in] matDict Lookup of the material that is assign to a each surface
  ///
  /// @return Acts layer array
  std::unique_ptr<const Acts::LayerArray>
  buildLayerArray(GeoVPhysVol const*                       vol,
                  std::shared_ptr<const Acts::Transform3D> transformationVolume,
                  bool                                     barrel, const MaterialDict& matDict) const;

  /// @brief This function builds the pixel detector barrel as
  /// Acts::TrackingVolume from a given GeoModel volume
  ///
  /// @param [in] pdBarrel The pixel detector barrel volume
  /// @param [in] index Index of this volume in the list of the parent
  /// @param [in] name Name of the volume that will be build - Allows further
  /// geometry deduction
  /// @param [in] matDict Lookup of the material that is assign to a each surface
  ///
  /// @return The Acts volume
  std::shared_ptr<Acts::TrackingVolume>
  buildVolume(GeoVPhysVol const* vol,
              unsigned int       index,
              std::string        name, const MaterialDict& matDict) const;

  /// @warning The tracing through the GeoModel is based purely on string
  /// parsing. Any careless change in the following lines can break the code.
  /// Set of keys we search for in the GeoModel to find the right volumes in the
  /// tree
  const std::set<std::string> m_volumeKeys = {"Barrel", "EndCap"};
  /// Set of keys we search for in the GeoModel to find the right layers in the
  /// volumes
  const std::set<std::string> m_layerKeys
      = {"Layer0", "Layer1", "Layer2", "Layer3", "Disk0", "Disk1", "Disk2"};
  /// Vector containing the volume names that will be produces
  const std::array<std::string, 2> m_outputVolumeNames
      = {"ACTlaS::Pixel::Barrel", "ACTlaS::Pixel::EndCap"};
};
}  // namespace FW