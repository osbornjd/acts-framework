// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <QString>
#include <iostream>
#include <set>
#include "Acts/Detector/TrackingVolume.hpp"

class GeoPcon;
class GeoVPhysVol;

namespace Acts {
class SurfaceMaterial;
}

namespace FW {

class GeoModelBeamPipe
{
public:
  /// @brief Creates a part of the beam pipe in Acts by calling the right
  /// function
  ///
  /// @param [in] bp Pointer to the beam pipe part
  ///
  /// @return Acts pointer to the corresponding beam pipe volume
  Acts::MutableTrackingVolumePtr
  buildBeamPipe(GeoVPhysVol const*                           bp,
                std::shared_ptr<const Acts::SurfaceMaterial> material
                = nullptr) const;

  /// @brief This function provides the bin boundaries for the material mapping
  ///
  /// @param [in] bp Pointer to the beam pipe part
  ///
  /// @return Set of bin boundaries along the z-axis
  std::set<double>
  beamPipeMaterialBinning(GeoVPhysVol const* bp) const;

private:
  /// @brief Constructs an Acts beam pipe out of a given GeoModel detector. The
  /// central beam pipe is represented by a single layer with binned surface
  /// material attached.
  /// @note There is no material attached yet
  ///
  /// @param [in] bp Pointer to the beam pipe
  /// @param [in] material Optional material assigned to the pipe
  ///
  /// @return Acts pointer to the beam pipe volume
  Acts::MutableTrackingVolumePtr
  buildCentralBeamPipe(GeoVPhysVol const*                           bp,
                       std::shared_ptr<const Acts::SurfaceMaterial> material
                       = nullptr) const;

  /// @brief Constructs an Acts forward beam pipe out of a given GeoModel
  /// detector. Along the z-direction a single layer represents the beam pipe
  /// parts. Since the beam pipe diameter can become bigger towards the outer
  /// side, these parts are described by cones with binned surface material
  /// attached to the parts. The total envelope is given by a cylinder, assuming
  /// that it does not intersect with outher (relevant) detector volumes.
  /// @note There is no material attached yet
  ///
  /// @param [in] bp Pointer to the forward beam pipe
  /// @param [in] material Optional material assigned to the pipe
  ///
  /// @return Acts pointer to the forward beam pipe volume
  Acts::MutableTrackingVolumePtr
  buildFwdBeamPipe(GeoVPhysVol const*                           bp,
                   std::shared_ptr<const Acts::SurfaceMaterial> material
                   = nullptr) const;

  std::vector<std::shared_ptr<const Acts::Layer>>
  pconLayerVector(const Acts::Transform3D& trafoToVolume,
                  GeoPcon const*           pcon,
                  std::pair<double, double>& minMaxZ,
                  std::pair<double, double>& minMarR) const;

  /// @brief Extracts the half length of a tube
  ///
  /// @param [in] gvpv Pointer to the surface
  ///
  /// @return The half length
  double
  tubeHalfLength(GeoVPhysVol const* gvpv) const;
};
}  // namespace FW