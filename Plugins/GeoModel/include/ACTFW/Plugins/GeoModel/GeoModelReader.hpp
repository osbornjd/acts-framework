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

class GeoFullPhysVol;
class GeoPhysVol;
class GeoVPhysVol;
class GeoLogVol;
class GeoShape;

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

  /// @brief Printer of the full detector
  std::ostream&
  treeToStream(GeoVPhysVol const* tree, std::ostream& sl) const;

  /// @brief Printer of GeoPhysVol
  std::ostream&
  toStream(GeoPhysVol const* gpv, std::ostream& sl) const;

  /// @brief Printer of GeoFullPhysVol
  std::ostream&
  toStream(GeoFullPhysVol const* gfpv, std::ostream& sl) const;

private:
  /// @brief Printer of GeoLogVol
  std::ostream&
  toStream(GeoLogVol const* glv, std::ostream& sl) const;

  /// @brief Printer of the shape
  std::ostream&
  shapeToStream(GeoShape const* shape, std::ostream& sl) const;
};
}  // namespace FW