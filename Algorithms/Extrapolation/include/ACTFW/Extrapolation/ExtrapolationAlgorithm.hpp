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
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"

namespace Acts {
 class IExtrapolationEngine;
 class TrackingGeometry;
 class TrackingVolume;
}

namespace FW {
 class WhiteBoard;
}

namespace FWA {

/// @class ExtrapolationAlgorithm
class ExtrapolationAlgorithm : public FW::Algorithm
{
public:
  /// @class Config
  struct Config : public FW::Algorithm::Config
  {
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    /// the extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;
    /// output writer for charged particles
    std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters> > >
      ecChargedWriter = nullptr;
    /// output writer for charged particles
    std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::NeutralParameters> > >
      ecNeutralWriter = nullptr;
    /// output writer for material
    std::shared_ptr< FW::IWriterT<Acts::MaterialTrack> >
      materialWriter = nullptr; 
    /// the tracking geometry
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// if this is set then the particles are taken from the collection
    std::string evgenParticlesCollection = "";
    /// if this is set then the particles are written to the event store
    std::string simulatedParticlesCollection = "";
    /// if this is set then the hit collection is written
    std::string simulatedHitsCollection = "";
    /// the cuts applied in this case
    /// @todo remove later and replace by particle selector
    double maxEta = 3.;
    double minPt  = 250.;
    /// number of tests per event
    size_t testsPerEvent = 1;
    /// parameter type : 0 = neutral | 1 = charged
    int parameterType = 1;
    /// mean, sigma for d0 range
    std::array<double, 2> d0Defs = {{0., 2.}};
    /// mean, sigma for z0 range
    std::array<double, 2> z0Defs = {{0., 50.}};
    /// low, high for eta range
    std::array<double, 2> etaRange = {{-3., 3.}};
    /// low, high for phi range
    std::array<double, 2> phiRange = {{-M_PI, M_PI}};
    /// low, high for pt range
    std::array<double, 2> ptRange = {{100., 10000.}};
    /// particle type definition
    bool particleType = true;
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

    Config() : FW::Algorithm::Config("ExtrapolationAlgorithm") {}
  };

  /// Constructor
  ExtrapolationAlgorithm(
      const Config&                       cnf,
      std::unique_ptr<const Acts::Logger> logger
      = Acts::getDefaultLogger("ExtrapolationAlgorithm",
                               Acts::Logging::INFO));

  /// Destructor
  ~ExtrapolationAlgorithm();

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

  double
  drawGauss(FW::RandomNumbersSvc::Generator& rng,
            const std::array<double, 2>& range) const;
  double
  drawUniform(FW::RandomNumbersSvc::Generator& rng,
              const std::array<double, 2>& range) const;

  template <class T>
  FW::ProcessCode
  executeTestT(const T& startParameters,
               barcode_type barcode = 0,
               FW::DetectorData<geo_id_value, 
               std::pair< std::unique_ptr<const T>, barcode_type >  >* dData = nullptr,
               std::shared_ptr< FW::IWriterT< Acts::ExtrapolationCell<T> > > writer = nullptr) const;

};
}
#include "ExtrapolationAlgorithm.ipp"

#endif //ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
