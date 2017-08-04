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
#include <limits>
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
    /// the tracking geometry
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// the particles input collections
    std::string particlesCollection              = "";
    /// the simulated particles output collection
    std::string simulatedParticlesCollection     = "";
    /// the simulated hits output collection
    std::string simulatedHitsCollection          = "";
    /// the simulated charged excell collection
    std::string simulatedChargedExCellCollection = "";
    /// the simulated neutral excell collection
    std::string simulatedNeutralExCellCollection = "";
    /// the cuts applied in this case
    /// @todo remove later and replace by particle selector
    double maxD0  = std::numeric_limits<double>::max();
    double maxEta = std::numeric_limits<double>::max();;
    double minPt  = 0.0;
    /// skip or process neutral particles
    bool skipNeutral          = false;
    /// configuration: sensitive collection
    bool collectSensitive     = true;
    /// configuration: collect passive
    bool collectPassive       = true;
    /// configuration: collect boundary
    bool collectBoundary      = true;
    /// configuration: collect material
    bool collectMaterial      = true;
    /// configuration: don't collapse
    bool sensitiveCurvilinear = false;
    /// define how robust the search mode is
    int searchMode            = 0;
    /// set the patch limit of the extrapolation
    double pathLimit          = -1.;
  };

  /// Constructor
  ExtrapolationAlgorithm(const Config& cnf);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  execute(AlgorithmContext ctx) const final override;

private:
  Config m_cfg;  ///< the config class

  /// the templated execute test method for 
  /// charged and netural particles 
  /// @param [in] the start parameters
  /// @param [in] the particle barcode
  /// @param [in] the detector data container 
  template <class T>
  ProcessCode
  executeTestT(
      const T&     startParameters,
      barcode_type barcode,
      std::vector< Acts::ExtrapolationCell< T> >& eCells,
      DetectorData<geo_id_value,
                   std::pair<std::unique_ptr<const T>, barcode_type>>* dData
                    = nullptr) const;
};
}  // namespace FW
#include "ExtrapolationAlgorithm.ipp"

#endif  // ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
