// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <climits>
#include <memory>
#include <mutex>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "Acts/Plugins/MaterialMapping/SurfaceMaterialMapper.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace Acts {
class TrackingGeometry;
using IndexedSurfaceMaterial
    = std::pair<GeometryID, std::unique_ptr<const SurfaceMaterial>>;
}

namespace FW {

/// @class MaterialMapping
///
/// @brief Initiates and executes material mapping
///
/// The MaterialMapping reads in the MaterialTrack with a dedicated
/// reader and uses the material mapper to project the material onto
/// the tracking geometry
///
/// By construction, the material mapping needs inter-event information
/// to build the material maps of accumulated single particle views.
/// However, running it in one single event, puts enormous pressure onto
/// the I/O structure.
///
/// It therefore saves the mapping state/cache as a private member variable
/// and is designed to be executed in a single threaded mode.
class MaterialMapping : public FW::BareAlgorithm
{
public:
  /// @class nested Config class
  /// of the MaterialMapping algorithm
  struct Config
  {
  public:
    /// Input collection
    std::string collection = "materialtracks";

    /// The ACTS surface material mapper
    std::shared_ptr<Acts::SurfaceMaterialMapper> materialMapper = nullptr;

    /// The writer of the material
    std::shared_ptr<FW::IWriterT<Acts::IndexedSurfaceMaterial>>
        indexedMaterialWriter = nullptr;

    /// The TrackingGeometry to be mapped on
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
  };

  /// Constructor
  ///
  /// @param cfg The configuration struct carrying the used tools
  /// @param level The output logging level
  MaterialMapping(const Config&        cfg,
                  Acts::Logging::Level level = Acts::Logging::INFO);

  /// Destructor
  /// - it also writes out the file
  ~MaterialMapping();

  /// Framework execute method
  ///
  /// @param context The algorithm context for event consistency
  FW::ProcessCode
  execute(FW::AlgorithmContext context) const final override;

private:
  Config m_cfg;  //!< internal config object
  Acts::SurfaceMaterialMapper::State
      m_mappingState;  //!< Material mapping state
};

}  // namespace FW
