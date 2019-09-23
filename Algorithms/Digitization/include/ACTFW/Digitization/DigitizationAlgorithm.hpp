// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/detail/Axis.hpp"
#include "Acts/Utilities/detail/Grid.hpp"

namespace Acts {
class PlanarModuleStepper;
class TrackingGeomtetyr;
}  // namespace Acts

namespace FW {

// clang-format off
/// grid that holds the resolution as estimated given the cluster sizes in one or both directions
typedef Acts::detail::Grid<double, Acts::detail::EquidistantAxis, Acts::detail::EquidistantAxis> ResolutionGrid;
typedef std::pair<ResolutionGrid,ResolutionGrid> LayerResolution;
typedef std::map<Acts::GeometryID, std::shared_ptr<LayerResolution> > ResolutionMap;
/// clang-format on

/// Algorithm to create planar clusters from fast or full simulation hits.
///
/// If a resolution file is given, this is used either
/// to smear with a gaussian distribution, or to fill
/// the covariance matrix
class DigitizationAlgorithm : public BareAlgorithm
{
public:
  /// Nested configuration struct
  struct Config
  {
    /// input hit collection
    std::string simulatedHitCollection;
    /// output clusters collection
    std::string clusterCollection;
    /// input resolution file, needed for gaussian smearing
    std::string resolutionFile;
    /// FW random number service
    std::shared_ptr<RandomNumbers> randomNumberSvc = nullptr;
    /// module stepper from ACTS core for geometric clustering
    std::shared_ptr<Acts::PlanarModuleStepper> planarModuleStepper = nullptr;
  };

  /// Constructor of the digitization algorithm
  ///
  /// @param cfg is the config struct to configure the algorihtm
  /// @param level is the logging level
  DigitizationAlgorithm(const Config&        cfg,
                        Acts::Logging::Level level = Acts::Logging::INFO);

                        
  /// Framework execute method of the digitization algorithm
  ///
  /// @param context is the algorithm context that holds event-wise information
  /// @return a process code to steer the algporithm flow
  ProcessCode
  execute(const AlgorithmContext& context) const final override;

private:
  Config m_cfg; /// config struct
};

}  // namespace FW
