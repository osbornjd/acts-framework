//
//  ExtrapolationAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
#define ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H

#include <cmath>
#include <memory>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTS/Utilities/GeometryID.hpp"

namespace Acts {
class IExtrapolationEngine;
class TrackingGeometry;
class TrackingVolume;
}  // namespace Acts

namespace FW {

class ExtrapolationAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// FW random number service
    std::shared_ptr<RandomNumbersSvc> randomNumbers = nullptr;
    /// the extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;
    /// output writer for charged particles
    std::shared_ptr<IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters>>>
        ecChargedWriter = nullptr;
    /// output writer for charged particles
    std::shared_ptr<IWriterT<Acts::ExtrapolationCell<Acts::NeutralParameters>>>
        ecNeutralWriter = nullptr;
    /// output writer for material
    std::shared_ptr<IWriterT<Acts::MaterialTrack>> materialWriter = nullptr;
    /// the tracking geometry
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// the particles input collections
    std::string particlesCollection;
    /// the simulated particles output collection
    std::string simulatedParticlesCollection;
    /// the simulated hits output collection
    std::string simulatedHitsCollection;
    /// the cuts applied in this case
    /// @todo remove later and replace by particle selector
    double maxD0  = 1.;
    double maxEta = 3.;
    double minPt  = 250.;
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
    /// define how robust the search mode is
    int searchMode = 0;
    /// set the patch limit of the extrapolation
    double pathLimit = -1.;
  };

  /// Constructor
  ExtrapolationAlgorithm(const Config& cnf);

  /// Framework execode method
  ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  Config m_cfg;  ///< the config class

  template <class T>
  ProcessCode
  executeTestT(
      const T&     startParameters,
      barcode_type barcode = 0,
      DetectorData<geo_id_value,
                   std::pair<std::unique_ptr<const T>, barcode_type>>* dData
      = nullptr,
      std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<T>>> writer
      = nullptr) const;
};
}  // namespace FWA
#include "ExtrapolationAlgorithm.ipp"

#endif  // ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
