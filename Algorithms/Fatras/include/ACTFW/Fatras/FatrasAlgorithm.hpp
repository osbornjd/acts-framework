//////////////////////////////////////////////////////////////////
// FatrasAlgorithm.hpp, Acts project
///////////////////////////////////////////////////////////////////
#ifndef ACTFW_ALGORITHMS_FATRASALGORITHM_H
#define ACTFW_ALGORITHMS_FATRASALGORITHM_H

#include <cmath>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/IMaterialEffectsEngine.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/MaterialInteraction.hpp"

namespace Acts {
class IExtrapolationEngine;
class TrackingGeometry;
class TrackingVolume;
}

namespace FW {
class WhiteBoard;
}

namespace FWA {

/// @class FatrasAlgorithm
template <class MaterialEngine>
class FatrasAlgorithm : public FW::Algorithm
{
public:
  /// @class Config
  struct Config : public FW::Algorithm::Config
  {
    /// The extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;
    /// The default logger
    std::shared_ptr<const Acts::Logger> logger;
    /// The name of the tool
    std::string name;
    /// define how robust the search mode is
    int searchMode = 0;
    /// configuration: sensitive collection
    bool collectSensitive = true;
    /// configuration: collect passive
    bool collectPassive = true;
    /// configuration: collect boundary
    bool collectBoundary = true;
    /// configuration: collect material
    bool collectMaterial = true;
    /// configuration: don't collapse
    bool sensitiveCurvilinear = false;
    /// set the patch limit of the extrapolation
    double pathLimit = -1.;
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    /// output writer for material
    std::shared_ptr<FW::IWriterT<Acts::MaterialTrack>> materialWriter = nullptr;
    /// the tracking geometry
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// if this is set then the particles are taken from the collection
    std::string evgenParticlesCollection = "";
    /// if this is set then the particles are written to the event store
    std::string simulatedParticlesCollection = "";
    /// if this is set then the hit collection is written
    std::string simulatedHitsCollection = "";
    /// if this is set the extrapolation cell is written to the event store
    std::string simulatedExCell = "";
    /// the cuts applied in this case
    /// @todo remove later and replace by particle selector
    double maxEta = 3.;
    double minPt  = 250.;
    /// number of tests per event
    size_t testsPerEvent = 1;
    /// parameter type : 0 = neutral | 1 = charged
    int parameterType = 1;
    /// The material interaction engine
    /// @note This engine must be the same engine used to create the
    /// extrapolation engine
    std::shared_ptr<MaterialEngine> materialInteractionEngine = nullptr;
    /// @todo only for validation - remove afterwards!
    /// output writer for charged particles
    std::
        shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters>>>
            ecChargedWriter = nullptr;

    Config() : FW::Algorithm::Config("FatrasAlgorithm") {}
  };

  /// Constructor
  FatrasAlgorithm(const Config&                       cnf,
                  std::unique_ptr<const Acts::Logger> logger
                  = Acts::getDefaultLogger("FatrasAlgorithm",
                                           Acts::Logging::INFO));

  /// Destructor
  ~FatrasAlgorithm() = default;

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final override;

  /// Framework execode method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const final override;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final override;

private:
  Config m_cfg;  ///< the config class
};
}

#include "FatrasAlgorithm.ipp"

#endif  // ACTFW_ALGORITHMS_FATRASALGORITHM_H
