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
/// The HitDistanceAlgorithm receives a map which containes a collection of
/// measurements for each sensitive surface. It calculates the distances (for
/// both local components) of all measurements coming from different particles
/// on the same surface. Finally two different outputs for later analysis are
/// provided:
/// - surfaceHitAnalysis (1)
/// - layerHitAnalysis   (2)
/// (1) Calculates the minima, maxima and averages of the hit distances on each
/// surface for a given event.
/// (2) Calculates the minima, maxima and averages of the hit distances on each
/// layer for a given event.
/// The ouput can be written out using the
/// FW::Root::RootHitDistanceAnalysisWriter.hpp
///
class HitDistanceAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// the trackHits input collections
    std::string collection = "measurements";
    /// The output parameters of the hit distance analysis for each surface
    std::string surfaceHitAnalysis = "surfaceHitAnalysis";
    /// The output parameters of the hit distance analysis for each layer
    std::string layerHitAnalysis = "layerHitAnalysis";
    /// The truth particle map
    std::string particleMap = "particleMap";
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
