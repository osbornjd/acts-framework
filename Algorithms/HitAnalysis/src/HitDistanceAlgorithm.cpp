#include "ACTFW/HitAnalysis/HitDistanceAlgorithm.hpp"
#include <boost/iterator/counting_iterator.hpp>
#include <map>
#include <numeric>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTS/Layers/Layer.hpp"
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
  // read measurements from input collection
  const std::vector<FW::fccMeasurement>* measurements = nullptr;
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
  // read truth particle map from input collection
  const std::map<unsigned, const FW::fccTruthParticle>* particleMap = nullptr;
  if (ctx.eventStore.get(m_cfg.particleMap, particleMap)
      == FW::ProcessCode::ABORT) {
    ACTS_INFO("Could not find collection: " << m_cfg.collection
                                            << " in event store. Abort.");
    return FW::ProcessCode::ABORT;
  }
  if (!particleMap) {
    ACTS_DEBUG("No track hits to be processed for this event");
    return FW::ProcessCode::SUCCESS;
  } else
    std::cout << "Found map with size: " << particleMap->size() << std::endl;
  // map containing all measurements of each surface for this event
  auto measOnSurfaces
      = std::map<Acts::GeometryID, std::vector<FW::fccMeasurement>>();
  // get all local positions for each surface
  for (auto& measurement : *measurements) {
    // get the surface ID
    auto surfaceID = measurement.measurement().referenceSurface().geoID();
    // push back measurements for each surface
    measOnSurfaces[surfaceID].push_back(measurement);
  }

  // map containing all distances of each layer for both local coordinates + the
  // position of the corresponding surface
  std::map<Acts::GeometryID,
           std::vector<std::pair<Acts::Vector2D, Acts::Vector3D>>>
      distancesOnLayer;
  // the hit analysis parameters & averageR/averageZ position for each surface
  std::map<Acts::GeometryID,
           std::tuple<FW::AnalysisParameters,
                      FW::AnalysisParameters,
                      double,
                      double>>
      hitAnalysisPerSurface;

  // go through surfaces to calculate distances
  for (auto& surf : measOnSurfaces) {
    // get the corresponding layer ID
    auto layerID = surf.first.value(Acts::GeometryID::volume_mask
                                    | Acts::GeometryID::layer_mask);
    // get all measurments of this surface
    auto measurements = surf.second;
    // parameters for the analysis
    // sums
    double surf_sum0 = 0;
    double surf_sum1 = 0;
    size_t nHits     = 0;
    // extrema
    double surf_min0 = std::numeric_limits<double>::max();
    double surf_max0 = std::numeric_limits<double>::min();
    double surf_min1 = std::numeric_limits<double>::max();
    double surf_max1 = std::numeric_limits<double>::min();
    // we are on one surface - surface center is the same for all positions
    auto surfaceCenter
        = measurements.begin()->measurement().referenceSurface().center();
    // eta & phi of surface
    double eta = surfaceCenter.eta();
    double phi = surfaceCenter.phi();

    // go through positions and calculate distances
    for (auto ref = measurements.begin(); ref != (measurements.end() - 1);
         ref++) {
      // go through all the measurements to calculate the distances to the
      // remaining measurements (ref is the current reference measurement)
      for (auto meas = (ref + 1); meas != measurements.end(); meas++) {
        // search for the particle in the truth (particle below a certain energy
        // are not written to truth)
        auto searchParticle = particleMap->find(meas->trackID());
        // only calculate distance, if the measurement comes from another
        // particle -> if the trackID differs, if the particle is found in the
        // truth and if it is a primary
        if (meas->trackID() != ref->trackID()
            && searchParticle != particleMap->end()
            && searchParticle->second.status() == 1) {
          // caclulate the distances of the two coordinates
          double d0 = fabs(meas->locPos().x() - ref->locPos().x());
          double d1 = fabs(meas->locPos().y() - ref->locPos().y());
          // calculate minima & maxima & sums
          surf_sum0 += d0;
          surf_sum1 += d1;
          if (d0 < surf_min0) surf_min0 = d0;
          if (d0 > surf_max0) surf_max0 = d0;
          if (d1 < surf_min1) surf_min1 = d1;
          if (d1 > surf_max1) surf_max1 = d1;
          nHits++;
          // write out distances per layer
          distancesOnLayer[layerID].push_back(
              std::make_pair(Acts::Vector2D(d0, d1), surfaceCenter));
        }
      }
    }

    if (nHits > 1) {
      // calculate averages
      double surf_mean0 = surf_sum0 / nHits;
      double surf_mean1 = surf_sum1 / nHits;

      // create analysis parameters for surfaces
      FW::AnalysisParameters surf_distanceParameters0(
          surf_mean0, surf_min0, surf_max0);
      FW::AnalysisParameters surf_distanceParameters1(
          surf_mean1, surf_min1, surf_max1);
      // write analysis parameters per surface on a map
      hitAnalysisPerSurface.emplace(std::make_pair(
          surf.first,
          (std::make_tuple(
              surf_distanceParameters0, surf_distanceParameters1, eta, phi))));
    }
  }

  // the hit analysis parameters & averageR/averageZ position for each layer
  std::map<Acts::GeometryID,
           std::tuple<FW::AnalysisParameters,
                      FW::AnalysisParameters,
                      double,
                      double>>
      hitAnalysisPerLayer;

  // go through layers to caclulate averages, minima & maxima for the layers
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
    // average position
    double averageR = 0.;
    double averageZ = 0.;
    // go through distances and calculate parameters
    for (auto& distance : distances) {
      sum0 += distance.first.x();
      sum1 += distance.first.y();
      if (distance.first.x() < min0) min0 = distance.first.x();
      if (distance.first.x() > max0) max0 = distance.first.x();
      if (distance.first.y() < min1) min1 = distance.first.y();
      if (distance.first.y() > max1) max1 = distance.first.y();

      averageR += distance.second.perp();
      averageZ += distance.second.z();
    }
    if (distances.size()) {
      // get the means
      double mean0 = sum0 / double(distances.size());
      double mean1 = sum1 / double(distances.size());
      averageR /= double(distances.size());
      averageZ /= double(distances.size());
      // create analysis parameters
      FW::AnalysisParameters distanceParameters0(mean0, min0, max0);
      FW::AnalysisParameters distanceParameters1(mean1, min1, max1);
      // create analysis parameters per layer
      hitAnalysisPerLayer.emplace(
          layer.first,
          std::make_tuple(
              distanceParameters0, distanceParameters1, averageR, averageZ));
    }
  }

  // write to event store
  if (ctx.eventStore.add(m_cfg.surfaceHitAnalysis,
                         std::move(hitAnalysisPerSurface))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  // write to event store
  if (ctx.eventStore.add(m_cfg.layerHitAnalysis, std::move(hitAnalysisPerLayer))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}
