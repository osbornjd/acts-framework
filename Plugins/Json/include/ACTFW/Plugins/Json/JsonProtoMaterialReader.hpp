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
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "Acts/Detector/TrackingVolume.hpp"
#include "Acts/Material/IMaterialDecorator.hpp"
#include "Acts/Material/ISurfaceMaterial.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace Acts {

using SurfaceMaterialMap
    = std::map<GeometryID, std::shared_ptr<const ISurfaceMaterial>>;
}

namespace FW {

namespace Json {

  /// @class JsonProtoSurfaceMaterialReader
  ///
  /// @brief Read the collection of SurfaceMaterial from a file in order to
  /// load it onto the TrackingGeometry
  class JsonProtoSurfaceMaterialReader
      : public FW::IReaderT<Acts::SurfaceMaterialMap>
  {
  public:
    /// @class Config
    /// Configuration of the Reader
    class Config
    {
    public:
      /// The volume identification string
      std::string voltag = "vol";
      /// The layer identification string
      std::string laytag = "lay";
      /// The approach identification string
      std::string apptag = "app";
      /// The sensitive identification string
      std::string sentag = "sen";
      /// The value tag -> binning values: binZ, binR, binPhi, etc.
      std::string vtag = "v";
      /// The bin position tag
      std::string ntag = "n";
      /// The option tag -> binning options: open, closed
      std::string otag = "o";
      /// The name of the output file
      std::string fileName = "proto-maps.json";
      /// The default logger
      std::shared_ptr<const Acts::Logger> logger;
      // The name of the writer
      std::string name = "";

      /// Constructor
      ///
      /// @param lname Name of the writer tool
      /// @param lvl The output logging level
      Config(const std::string&   lname = "ProtoMaterialReader",
             Acts::Logging::Level lvl   = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
      {
      }
    };

    /// Constructor
    ///
    /// @param cfg configuration struct for the reader
    JsonProtoSurfaceMaterialReader(const Config& cfg);

    /// Destructor
    ~JsonProtoSurfaceMaterialReader() override = default;

    /// Framework name() method
    std::string
    name() const final override;

    /// Read method
    ///
    /// @param surfaceMaterialMap The indexed material map collection
    /// @param skip is the number of skip reads (0 for this reader)
    /// @param is the AlgorithmContext pointer in case the reader would need
    /// information about the event context (not true in this case)
    FW::ProcessCode
    read(Acts::SurfaceMaterialMap&   sMaterialMap,
         size_t                      skip = 0,
         const FW::AlgorithmContext* ctx  = nullptr) final override;

  private:
    /// The config class
    Config m_cfg;

    /// mutex used to protect multi-threaded reads
    std::mutex m_read_mutex;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

  inline std::string
  JsonProtoSurfaceMaterialReader::name() const
  {
    return m_cfg.name;
  }

  ///@brief Material decorator from ROOT
  class JsonProtoMaterialDecorator : public Acts::IMaterialDecorator
  {
  public:
    JsonProtoMaterialDecorator(JsonProtoSurfaceMaterialReader::Config rConfig,
                               bool clearSurfaceMaterial = true,
                               bool clearVolumeMaterial  = true)
      : m_readerConfig(rConfig)
      , m_clearSurfaceMaterial(clearSurfaceMaterial)
      , m_clearVolumeMaterial(clearVolumeMaterial)
    {
      // Create the reader with the config
      JsonProtoSurfaceMaterialReader sreader(m_readerConfig);
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
    JsonProtoSurfaceMaterialReader::Config m_readerConfig;
    Acts::SurfaceMaterialMap               m_surfaceMaterialMap;

    bool m_clearSurfaceMaterial{true};
    bool m_clearVolumeMaterial{true};
  };

}  // namespace Json
}  // namespace FW
