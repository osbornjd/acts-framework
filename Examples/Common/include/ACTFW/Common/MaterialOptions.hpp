// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Detector/TrackingVolume.hpp"
#include "Acts/Material/IMaterialDecorator.hpp"
#include "Acts/Surfaces/Surface.hpp"

namespace po = boost::program_options;

namespace FW {

/// @brief standard material wiper
///
/// It removes the material of surfaces/volumes if configured
/// to do so
class MaterialWiper : public Acts::IMaterialDecorator
{
public:
  /// Create a material wiper
  ///
  /// @param sf is the surface wiping directive
  /// @param vol is the volume wiping directive
  MaterialWiper(bool sf = true, bool vol = true)
    : m_wipeSurfaceMaterial(sf), m_wipeVolumeMaterial(vol)
  {
  }

  /// Decorate a surface
  ///
  /// @param surface the non-cost surface that is decorated
  void
  decorate(Acts::Surface& surface) const final
  {
    surface.assignSurfaceMaterial(nullptr);
  }

  /// Decorate a TrackingVolume
  ///
  /// @param volume the non-cost volume that is decorated
  void
  decorate(Acts::TrackingVolume& volume) const final
  {
    volume.assignVolumeMaterial(nullptr);
  }

private:
  bool m_wipeSurfaceMaterial{true};
  bool m_wipeVolumeMaterial{true};
};

namespace Options {

  /// The options for the material loading
  template <typename aopt_t>
  void
  addMaterialOptions(aopt_t& opt)
  {
    opt.add_options()("mat-type",
                      po::value<size_t>()->default_value(0),
                      "The type for the material: 0 - none, 1 - proxy, 2 - "
                      "building, 3 - reading")(
        "mat-input-file",
        po::value<std::string>()->default_value(""),
        "The filename for the material, auto-detects loading plugin")(
        "mat-output-file",
        po::value<std::string>()->default_value(""),
        "The filename for the material, auto-detects loading plugin");
  }

  /// Read the material decorator
  ///
  /// @tparam amap_t Type of the options map
  ///
  /// @param[in] vm Map to be read in
  template <typename amap_t>
  std::shared_ptr<const Acts::IMaterialDecorator>
  readMaterialDecorator(const amap_t& vm)
  {

    if (vm["mat-type"].template as<size_t>() == 0) {
      return std::make_shared<const MaterialWiper>(true, true);
    }

    return nullptr;
  }

}  // namespace
}  // namespace
