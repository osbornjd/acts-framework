#ifndef ACTFW_ALGORITHMS_HITANALYSIS_HITDISTANCEALGORITHM_H
#define ACTFW_ALGORITHMS_HITANALYSIS_HITDISTANCEALGORITHM_H

#include <ACTS/Utilities/Units.hpp>
#include <array>
#include <map>
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Utilities/HitData.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Identifier.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
class GeometryID;
}

namespace FW {

/// @class HitDistanceAlgorithm
///
/// @brief Calculates hit distances per layer
///
/// The HitDistanceAlgorithm receives a collection of measurements. It
/// calculates the distances of all measurements on the same surface and then
/// calculates the mean, the minimum and maximum per layer for both local
/// components and writes them to the event store.
///
class HitDistanceAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// the trackHits input collections
    std::string collection = "measurements";
    /// The output parameters of the hit distance analysis for each layer
    std::string layerHitAnalysis = "layerHitAnalysis";
  };

  /// Constructor
  /// @param cfg is the configuration class
  HitDistanceAlgorithm(const Config&        cfg,
                       Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  ProcessCode
  execute(AlgorithmContext ctx) const;

private:
  /// The configuration object
  Config m_cfg;
};

}  // namespace FW

#endif  // ACTFW_ALGORITHMS_HITANALYSIS_HITDISTANCEALGORITHM_H
