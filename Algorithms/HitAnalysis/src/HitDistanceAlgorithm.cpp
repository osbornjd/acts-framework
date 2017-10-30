#include "ACTFW/HitAnalysis/HitDistanceAlgorithm.hpp"
#include <boost/iterator/counting_iterator.hpp>
#include <map>
#include <numeric>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
//#include "DD4hep/LCDD.h"

FW::HitDistanceAlgorithm::HitDistanceAlgorithm(const Config&        cfg,
                                               Acts::Logging::Level level)
  : FW::BareAlgorithm("ParticleGun", level), m_cfg(cfg)
{
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing name of measurement collection.");
  }
  if (m_cfg.layerHitAnalysis.empty()) {
    throw std::invalid_argument("Missing parameter 'layerHitAnalysis'");
  }
}

FW::ProcessCode
FW::HitDistanceAlgorithm::execute(FW::AlgorithmContext ctx) const
{
  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);
  // read particles from input collection
  const std::vector<FW::Measurement2D>* measurements = nullptr;
  if (ctx.eventStore.get(m_cfg.collection, measurements)
      == FW::ProcessCode::ABORT) {
    ACTS_INFO("Could not find collection: " << m_cfg.collection
                                            << " in event store. Abort.");
    return FW::ProcessCode::ABORT;
  }
  if (!measurements) {
    ACTS_DEBUG("No track hits to be processed for this event");
    return FW::ProcessCode::SUCCESS;
  }

  // map containing all local positions of each surface
  std::map<Acts::GeometryID, std::vector<Acts::Vector2D>> locPosOnSurfaces;
  // get all local positions for each surface
  for (auto& measurement : *measurements) {
    auto surfaceID = measurement.referenceSurface().geoID();
    locPosOnSurfaces[surfaceID].push_back(
        Acts::Vector2D(measurement.get<Acts::ParDef::eLOC_0>(),
                       measurement.get<Acts::ParDef::eLOC_1>()));
  }

  // map containing all distances of each layer
  std::map<Acts::GeometryID, std::vector<std::pair<float, float>>>
      distancesOnLayer;
  // go through surfaces to calculate distances
  for (auto& surf : locPosOnSurfaces) {
    // get the corresponding layer ID
    auto layerID = surf.first.value(Acts::GeometryID::volume_mask
                                    & Acts::GeometryID::layer_mask);
    // get all positions on surfaces
    auto localPositions = surf.second;
    // go through positions and calculate distances
    for (auto it = localPositions.begin(); it != (localPositions.end() - 1);
         it++) {
      for (auto locPos = (it + 1); locPos != localPositions.end(); locPos++) {
        double d0 = locPos->x() - it->x();
        double d1 = locPos->y() - it->y();
        // write out distances per layer
        distancesOnLayer[layerID].push_back(std::make_pair(d0, d1));
      }
    }
  }
  // the hit analysis parameters for each layer
  std::map<Acts::GeometryID,
           std::pair<FW::AnalysisParameters, FW::AnalysisParameters>>
      hitAnalysisPerLayer;
  // go through layers to caclulate averages, minima & maxima
  for (auto& layer : distancesOnLayer) {
    // get all distances for this layer
    auto distances = layer.second;
    // sums
    double sum0 = 0;
    double sum1 = 0;
    // extrema
    double min0 = std::numeric_limits<double>::max();
    double max0 = std::numeric_limits<double>::min();
    double min1 = std::numeric_limits<double>::max();
    double max1 = std::numeric_limits<double>::min();
    // go through distances and calculate parameters
    for (auto& distance : distances) {
      sum0 += distance.first;
      sum1 += distance.second;
      if (distance.first < min0) min0  = distance.first;
      if (distance.first > max0) max0  = distance.first;
      if (distance.second < min1) min1 = distance.second;
      if (distance.second > max1) max1 = distance.second;
    }
    // get the means
    double mean0 = sum0 / double(distances.size());
    double mean1 = sum1 / double(distances.size());
    // create analysis parameters
    FW::AnalysisParameters distanceParameters0(mean0, min0, max0);
    FW::AnalysisParameters distanceParameters1(mean1, min1, max1);
    // create analysis parameters per layer
    hitAnalysisPerLayer.emplace(
        layer.first, std::make_pair(distanceParameters0, distanceParameters1));
  }
  // write to event store
  if (ctx.eventStore.add(m_cfg.layerHitAnalysis, std::move(hitAnalysisPerLayer))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  return FW::ProcessCode::SUCCESS;
}
