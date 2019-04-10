// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "Acts/Detector/TrackingVolume.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoPhysVol.h"

#include <QString>

namespace FW {

class GeoModelReader
{
public:
  /// @brief Temporary detector build for debugging - Will be removed in the
  /// future
  GeoPhysVol*
  makeDebugDetector() const;

  /// @brief Loads a db file that contains the detector data and stores the
  /// result in the GeoModel data type GeoPhysVol*
  ///
  /// @param [in] path Path to the file
  ///
  /// @return Pointer to the loaded detector
  GeoPhysVol*
  loadDB(const QString& path) const;

  /// @brief Creates a world volume around a GeoModel detector
  ///
  /// @param [in] world Pointer to the storage of the detector data
  ///
  /// @return Pointer to the whole world
  GeoPhysVol*
  createTheExperiment(GeoPhysVol* world) const;

  /// @brief Creates a part of the beam pipe in Acts by calling the right
  /// function
  ///
  /// @param [in] bp Pointer to the beam pipe part
  ///
  /// @return Acts pointer to the corresponding beam pipe volume
  Acts::MutableTrackingVolumePtr
  BuildBeamPipe(GeoVPhysVol const* bp) const

      /// @brief Printer of the full detector
      std::ostream& treeToStream(GeoVPhysVol const* tree,
                                 std::ostream&      sl) const;

  /// @brief Printer of GeoPhysVol
  std::ostream&
  toStream(GeoPhysVol const* gpv, std::ostream& sl) const;

  /// @brief Printer of GeoFullPhysVol
  std::ostream&
  toStream(GeoFullPhysVol const* gfpv, std::ostream& sl) const;

private:
  /// @brief Constructs an Acts beam pipe out of a given GeoModel detector. The
  /// central beam pipe is represented by a single layer with binned surface
  /// material attached.
  /// @note There is no material attached yet
  ///
  /// @param [in] bp Pointer to the beam pipe
  ///
  /// @return Acts pointer to the beam pipe volume
  Acts::MutableTrackingVolumePtr
  buildCentralBeamPipe(GeoVPhysVol const* bp) const;

  /// @brief Constructs an Acts forward beam pipe out of a given GeoModel
  /// detector. Along the z-direction a single layer represents the beam pipe
  /// parts. Since the beam pipe diameter can become bigger towards the outer
  /// side, these parts are described by cones with binned surface material
  /// attached to the parts. The total envelope is given by a cylinder, assuming
  /// that it does not intersect with outher (relevant) detector volumes.
  /// @note There is no material attached yet
  ///
  /// @param [in] bp Pointer to the forward beam pipe
  ///
  /// @return Acts pointer to the forward beam pipe volume
  Acts::MutableTrackingVolumePtr
  buildFwdBeamPipe(GeoVPhysVol const* bp) const;

  /// @brief Extracts the half length of a tube
  ///
  /// @param [in] gvpv Pointer to the surface
  ///
  /// @return The half length
  double
  tubeHalfLength(GeoVPhysVol const* gvpv) const;

  /// @brief Printer of GeoLogVol
  std::ostream&
  toStream(GeoLogVol const* glv, std::ostream& sl) const;

  /// @brief Printer of the shape
  std::ostream&
  shapeToStream(GeoShape const* shape, std::ostream& sl) const;
};
}  // namespace FW
