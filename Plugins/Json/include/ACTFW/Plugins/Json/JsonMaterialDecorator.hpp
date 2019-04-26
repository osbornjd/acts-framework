// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include <mutex>
#include "ACTFW/Plugins/Json/JsonMaterialConverter.hpp"
#include "Acts/Material/IMaterialDecorator.hpp"
#include "Acts/Material/ISurfaceMaterial.hpp"
#include "Acts/Material/IVolumeMaterial.hpp"

// Convenience shorthand
using json = nlohmann::json;

namespace Acts {

using SurfaceMaterialMap
    = std::map<GeometryID, std::shared_ptr<const ISurfaceMaterial>>;

using VolumeMaterialMap
    = std::map<GeometryID, std::shared_ptr<const IVolumeMaterial>>;
}

namespace FW {

namespace Json {

  ///@brief Material decorator from ROOT
  class JsonProtoMaterialDecorator : public Acts::IMaterialDecorator
  {
  public:
    JsonProtoMaterialDecorator(JsonMaterialReader::Config rConfig,
                               bool clearSurfaceMaterial = true,
                               bool clearVolumeMaterial  = true)
      : m_readerConfig(rConfig)
      , m_clearSurfaceMaterial(clearSurfaceMaterial)
      , m_clearVolumeMaterial(clearVolumeMaterial)
    {
      // Create the reader with the config
      JsonMaterialReader sreader(m_readerConfig);
      // Read the map & return it
      sreader.read(m_surfaceMaterialMap);
    }

    /// Decorate a surface
    ///
    /// @param surface the non-cost surface that is decorated
    void
    decorate(Acts::Surface& surface) const final
    {
      // Clear the material if registered to do so
      if (m_clearSurfaceMaterial) {
        surface.assignSurfaceMaterial(nullptr);
      }

      // Try to find the surface in the map
      auto sMaterial = m_surfaceMaterialMap.find(surface.geoID());
      if (sMaterial != m_surfaceMaterialMap.end()) {
        surface.assignSurfaceMaterial(sMaterial->second);
      }
    }

    /// Decorate a TrackingVolume
    ///
    /// @param volume the non-cost volume that is decorated
    void
    decorate(Acts::TrackingVolume& volume) const final
    {
      // Clear the material if registered to do so
      if (m_clearVolumeMaterial) {
        volume.assignVolumeMaterial(nullptr);
      }
    }

  private:
    JsonMaterialReader::Config m_readerConfig;
    Acts::SurfaceMaterialMap   m_surfaceMaterialMap;

    bool m_clearSurfaceMaterial{true};
    bool m_clearVolumeMaterial{true};
  };

}  // namespace Json
}  // namespace FW
