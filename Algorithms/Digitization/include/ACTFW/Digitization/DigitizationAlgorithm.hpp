// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H
#define ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H

#include <memory>
#include <map>
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/detail/Axis.hpp"
#include "ACTS/Utilities/detail/Grid.hpp"

namespace Acts {
  class PlanarModuleStepper;
  class TrackingGeometry;
  typedef detail::Grid<double, detail::EquidistantAxis, detail::EquidistantAxis> ResolutionGrid;
  typedef std::pair<ResolutionGrid,ResolutionGrid> LayerResolution;
  typedef std::map<Acts::GeometryID, std::shared_ptr<Acts::LayerResolution> > ResolutionMap;
}

namespace FW {

class RandomNumbersSvc;

class DigitizationAlgorithm : public FW::BareAlgorithm
{
    
public:
  /// Nested configuration Config
  struct Config
  {
    /// input hit collection
    std::string simulatedHitsCollection;
    /// output space point collection
    std::string spacePointsCollection;
    /// output clusters collection
    std::string clustersCollection;
    /// FW random number service
    std::shared_ptr<RandomNumbersSvc> randomNumbers = nullptr;
    /// module stepper
    std::shared_ptr<Acts::PlanarModuleStepper> planarModuleStepper = nullptr;
    /// the reader for the resolution file, fixed to 2D for now
    std::shared_ptr<Acts::ResolutionMap> layerResolutions = nullptr;

    /// (@todo make cleaner after Tracking ML)

    /// the tracking geometry to make the module map
    std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
    /// read in by hand if the layer is of r-type and z-type     
    std::map< Acts::GeometryID, int > layerTypes;
    /// amount of smearing
    double smearParameter = 0.1; // accounts for n % gaussian width
    /// cut parameter - percentage of thickness to be traversed;
    double cutParameter   = 0.; //15; //  
    /// inefficiency for full hits
    double hitInefficiency = 0.01;
    /// maximum incidence angle cut-off 
    double cosThetaLocMin = 0.1;
    /// random/noise hits production probability per module
    // double randomHitsProb  = 0.5;
        
  };
  
  /// Constructor
  /// @param cnf the configuration class
  /// @param level the log output level
  DigitizationAlgorithm(const Config&        cnf,
                        Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execute method
  /// @param ctx the algorithm context to respect thread safety
  /// @return a process code 
  FW::ProcessCode
  execute(FW::AlgorithmContext ctx) const final override;

private:
  Config m_cfg;
};

}  // namespace FW

#endif
