// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Material/ISurfaceMaterial.hpp"
#include "Acts/Material/IVolumeMaterial.hpp"
#include "Acts/Material/MaterialProperties.hpp"
#include "Acts/Plugins/Json/lib/json.hpp"
#include "Acts/Utilities/BinUtility.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"

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

  /// @brief Layer representation for Json writing
  struct LayerRep
  {

    Acts::SurfaceMaterialMap                      sensitives;
    Acts::SurfaceMaterialMap                      approaches;
    std::shared_ptr<const Acts::ISurfaceMaterial> representing = nullptr;
  };

  /// @brief Volume representation for Json writing
  struct VolumeRep
  {

    std::map<geo_id_value, LayerRep> layers;
    Acts::SurfaceMaterialMap                     boundaries;
    std::shared_ptr<const Acts::IVolumeMaterial> material = nullptr;
  };

  /// @brief Detector representation for Json writing
  struct DetectorRep
  {

    std::map<geo_id_value, VolumeRep> volumes;
  };

  /// @class JsonGeometryConverter
  ///
  /// @brief read the material from Json
  class JsonGeometryConverter
  {

  public:
    /// @class Config
    /// Configuration of the Reader
    class Config
    {
    public:
      /// The geometry version
      std::string geoversion = "undefined";
      /// The detector tag
      std::string detkey = "detector";
      /// The volume identification string
      std::string volkey = "volumes";
      /// The boundary surface string
      std::string boukey = "boundaries";
      /// The layer identification string
      std::string laykey = "layers";
      /// The volume material string
      std::string matkey = "material";
      /// The approach identification string
      std::string appkey = "approach";
      /// The sensitive identification string
      std::string senkey = "sensitive";
      /// The representing idntification string
      std::string repkey = "representing";
      /// The bin keys
      std::string bin0key = "bin0";
      /// The bin1 key
      std::string bin1key = "bin1";
      /// The type key -> proto, else
      std::string typekey = "type";
      /// The data key
      std::string datakey = "data";
      /// The default logger
      std::shared_ptr<const Acts::Logger> logger;
      /// The name of the writer
      std::string name = "";

      /// Constructor
      ///
      /// @param lname Name of the writer tool
      /// @param lvl The output logging level
      Config(const std::string&   lname = "JsonGeometryConverter",
             Acts::Logging::Level lvl   = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
      {
      }
    };

    /// Constructor
    ///
    /// @param cfg configuration struct for the reader
    JsonGeometryConverter(const Config& cfg);

    /// Destructor
    ~JsonGeometryConverter() = default;

    /// Convert method
    ///
    /// @param surfaceMaterialMap The indexed material map collection
    std::pair<Acts::SurfaceMaterialMap, Acts::VolumeMaterialMap>
    jsonToMaterialMaps(const json& materialmaps);

    /// Convert method
    ///
    /// @param surfaceMaterialMap The indexed material map collection
    json
    materialMapsToJson(
        std::pair<Acts::SurfaceMaterialMap, Acts::VolumeMaterialMap>& maps);

    /// Write method
    ///
    /// @param tGeometry is the tracking geometry which contains the material
    // json trackingGeometryToJson(const TrackingGeometrt& tGeometry);

  private:
    /// Write method
    ///
    /// @param tGeometry is the tracking geometry which contains the material
    // json trackingVolumeToJson(const TrackingVolume& tVolume);

    /// Create the Surface Material from Json
    ///
    /// @param material is the json part representing a material object
    std::shared_ptr<const Acts::ISurfaceMaterial>
    jsonToSurfaceMaterial(const json& material);

    /// Create the Material Matrix from Json
    ///
    /// @param data is the json part representing a material data array
    Acts::MaterialPropertiesMatrix
    jsonToMaterialMatrix(const json& data);

    /// Create the BinUtility for from Json
    Acts::BinUtility
    jsonToBinUtility(const json& bin);

    /// Create Json from a detector represenation
    json
    detectorRepToJson(const DetectorRep& detRep);

    /// SurfaceMaterial to Json
    ///
    /// @param the SurfaceMaterial
    json
    surfaceMaterialToJson(const Acts::ISurfaceMaterial& sMaterial);

    /// MaterialPropertiesMatrix to json
    ///
    /// @param mpMatrix is the vector of vector of MaterialProperties
    /// json materialMatrixToJson(const Acts::MaterialPropertiesMatrix&
    /// mpMatrix);

    /// BinUtility to json
    ///
    /// @param bu The BinUtility to be converted
    /// json binUtilityToJson(const Acts::BinUtility& bu);

    /// The config class
    Config m_cfg;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

}  // namespace Json
}  // namespace FW