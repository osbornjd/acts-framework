///////////////////////////////////////////////////////////////////
// MaterialMapping.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_ALGORITHMS_MATERIALMAPPING_MATERIALMAPPING_H
#define ACTFW_ALGORITHMS_MATERIALMAPPING_MATERIALMAPPING_H

#include <climits>
#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Material/SurfaceMaterial.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialMapper.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace Acts {
class TrackingGeometry;
}

namespace FWA {

/// @class MaterialMapping
///
/// @brief Initiates material mapping
///
/// The MaterialMapping reads in the MaterialTrack with a dedicated
/// reader and uses the material mapper to project the material onto
/// the tracking geometry
///
/// In a final step, the material maps are written out for further usage

class MaterialMapping : public FW::BareAlgorithm
{
public:
  /// @class nested Config class
  /// of the MaterialMapping algorithm
  struct Config
  {
  public:
    /// The reader to read in the MaterialTrack entities
    std::shared_ptr<FW::IReaderT<Acts::MaterialTrack>> materialTrackReader
        = nullptr;
    /// The ACTS material mapper
    std::shared_ptr<Acts::MaterialMapper> materialMapper = nullptr;
    /// The validation writer of the material
    std::shared_ptr<FW::IWriterT<Acts::MaterialTrack>> materialTrackWriter
        = nullptr;
    /// The writer of the material
    std::shared_ptr<FW::IWriterT<Acts::IndexedSurfaceMaterial>>
        indexedMaterialWriter = nullptr;
    /// The TrackingGeometry to be mapped on
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// mapping conditions
    size_t maximumTrackRecords = std::numeric_limits<size_t>::infinity();
  };

  /// Constructor
  MaterialMapping(const Config&        cfg,
                  Acts::Logging::Level level = Acts::Logging::INFO);

  /// Destructor
  ~MaterialMapping();

  /// Framework execute method
  FW::ProcessCode
  execute(FW::AlgorithmContext context) const final;

private:
  Config m_cfg;
};

}  // namespace FWA

#endif  // ACTFW_ALGORITHMS_MATERIALMAPPING_MATERIALMAPPING_H
