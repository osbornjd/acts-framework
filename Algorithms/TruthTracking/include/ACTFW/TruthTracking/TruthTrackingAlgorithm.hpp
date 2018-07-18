// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_TRUTHTRACKING_H
#define ACTFW_ALGORITHMS_TRUTHTRACKING_H

#include <cmath>
#include <limits>
#include <memory>
#include <utility>
#include "ACTFW/Barcode/Barcode.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/Digitization/PlanarModuleCluster.hpp"
#include "Acts/EventData/Measurement.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Extrapolator/MaterialInteractor.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/Fitter/KalmanFitter.hpp"
#include "Acts/Fitter/KalmanUpdator.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Utilities/Units.hpp"
#include "Fatras/IEnergyLossSampler.hpp"
#include "Fatras/IHadronicInteractionSampler.hpp"
#include "Fatras/IMultipleScatteringSampler.hpp"
#include "Fatras/IPhotonConversionSampler.hpp"

namespace Acts {
class TrackingGeometry;
}  // namespace Acts

namespace FW {

/// @class TruthTrackingAlgorithm
///
/// The truth tracking algorithm reversed the hit association
/// and creates truth tracks that are then fitted with a track
/// fitter
template <typename Propagator_type>
class TruthTrackingAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {

    /// Constructor with propagator type
    Config(Propagator_type pgt) : propagator(std::move(pgt)) {}

    /// The templated propagator
    Propagator_type propagator;

    /// The TrackingGeometry
    std::shared_ptr<const TrackingGeometry> trackingGeometry = nullptr;

    /// the input cluster collection
    std::string clusterCollection = "";

    /// the output track collection
    std::string trackCollection = "";

    /// Fatras parameters, see FatrasOptions.hpp for details
    double       minPt   = 100. * Acts::units::_MeV;
    double       maxVr   = 1000. * Acts::units::_mm;
    double       maxVz   = 3000. * Acts::units::_mm;
    double       maxEta  = 5.;
    unsigned int minHits = 7;

    /// output debug mode for the extrapolator
    bool debugMode = false;
  };

  /// Constructor
  /// @param [in] cnf is the configuration struct
  /// @param [in] loglevel is the loggin level
  TruthTrackingAlgorithm(const Config& cnf, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  FW::ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  /// The config class
  Config m_cfg;
};

}  // namespace FW

#include "TruthTrackingAlgorithm.ipp"

#endif  // ACTFW_ALGORITHMS_TRUTHTRACKING_H
