// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>
#include <limits>
#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "Acts/EventData/NeutralParameters.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Extrapolation/ExtrapolationCell.hpp"
#include "Acts/Utilities/GeometryID.hpp"
#include "Acts/Utilities/Units.hpp"

namespace Acts {
class IExtrapolationEngine;
}  // namespace Acts

namespace FW {

/// @brief Algorithm that runs the legacy Extrapolation through a given detector
class ExtrapolationAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    /// the extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;
    /// the simulated charged excell collection (optional)
    std::string simulatedChargedExCellCollection = "";
    /// the simulated neutral excell collection (optional)
    std::string simulatedNeutralExCellCollection = "";
    /// how to set it up
    std::shared_ptr<RandomNumbersSvc> randomNumberSvc = nullptr;
    /// number of particles
    size_t nparticles = 100;
    /// d0 gaussian sigma
    double d0Sigma = 15. * Acts::units::_um;
    /// z0 gaussian sigma
    double z0Sigma = 55. * Acts::units::_mm;
    /// phi range
    std::pair<double, double> phiRange = {-M_PI, M_PI};
    /// eta range
    std::pair<double, double> etaRange = {-4., 4.};
    /// pt range
    std::pair<double, double> ptRange
        = {100. * Acts::units::_MeV, 100. * Acts::units::_GeV};
    /// skip or process neutral particles
    bool skipNeutral = false;
    /// configuration: sensitive collection
    bool resolveSensitive = true;
    /// configuration: collect passive
    bool resolvePassive = true;
    /// configuration: collect boundary
    bool collectBoundary = true;
    /// configuration: collect material
    bool resolveMaterial = true;
    /// configuration: don't collapse
    bool sensitiveCurvilinear = false;
    /// define how robust the search mode is
    int searchMode = 0;
    /// set the patch limit of the extrapolation
    double pathLimit = -1.;
  };

  /// Constructor
  /// @param [in] cfg is the configuration struct
  /// @param [in] loglevel is the loggin level
  ExtrapolationAlgorithm(const Config& cfg, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  FW::ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  Config m_cfg;  ///< the config struct

  /// the templated execute test method for
  /// charged and netural particles
  /// @param [in] startParameters the start parameters
  /// @param [in] eCells is the collection of extrapolation cells
  template <class T>
  ProcessCode
  executeTest(const T&                                 startParameters,
              std::vector<Acts::ExtrapolationCell<T>>& eCells) const;
};

}  // namespace FW
#include "ExtrapolationAlgorithm.ipp"
