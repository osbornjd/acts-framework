///////////////////////////////////////////////////////////////////
// MaterialMappingAlgorithm.hpp
///////////////////////////////////////////////////////////////////

#ifndef MATERIALMAPPINGTEST_MATERIALMAPPINGALGORITHM_H
#define MATERIALMAPPINGTEST_MATERIALMAPPINGALGORITHM_H

#include <memory>
#include <climits>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IMaterialTrackRecReader.hpp"
#include "ACTFW/Writers/IMaterialWriter.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Plugins/MaterialPlugins/SurfaceMaterialRecord.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace Acts {
class MaterialMapping;
class TrackingGeometry;
}

namespace FWE {

/// @class MaterialMappingAlgorithm
///
/// @brief Initiates material mapping
///
/// The MaterialMappingAlgorithm reads in the MaterialTrackRecord entities
/// with the MaterialTrackRecReader and initiates the mapping on the layers
/// with the ACTS MaterialMapping tool. It averages the material once every run
/// and initiates assigning of the final material to the layers.

class MaterialMappingAlgorithm : public FW::Algorithm
{
public:
  /// @class Config
  /// of the MaterialMappingAlgorithm

  struct Config : public FW::Algorithm::Config
  {
  public:
    /// The reader to read in the MaterialTrackRecord entities
    std::shared_ptr<FW::IMaterialTrackRecReader>  materialTrackRecReader;
    /// The ACTS material mapper
    std::shared_ptr<Acts::MaterialMapping>        materialMapper;
    /// The writer of the material
    std::shared_ptr<FW::IMaterialWriter>          materialWriter;
    /// The TrackingGeometry to be mapped on
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry;
    /// mapping conditions
    size_t                                        maximumTrackRecords;  

    Config()
      : FW::Algorithm::Config("MaterialRootification")
      , materialTrackRecReader(nullptr)
      , materialMapper(nullptr)
      , materialWriter(nullptr)
      , trackingGeometry(nullptr)
      , maximumTrackRecords(std::numeric_limits<size_t>::infinity())    
    {
    }
  };

  /// Constructor
  MaterialMappingAlgorithm(const Config&                 cfg,
                           std::unique_ptr<Acts::Logger> logger
                           = Acts::getDefaultLogger("MaterialMappingAlgorithm",
                                                    Acts::Logging::INFO));

  /// Destructor
  ~MaterialMappingAlgorithm();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;

  /// Framework execute method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

private:
  /// The config object
  Config m_cfg;
};
}

#endif  // MATERIALMAPPINGTEST_MATERIALMAPPINGALGORITHM_H
