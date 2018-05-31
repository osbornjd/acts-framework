// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_STRIPSPACEPOINTBUILDER_H
#define ACTFW_ALGORITHMS_STRIPSPACEPOINTBUILDER_H

#include <ACTS/Digitization/PlanarModuleCluster.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTS/Tools/SingleHitSpacePointBuilder.hpp"
#include "ACTS/Tools/DoubleHitSpacePointBuilder.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// Type of the hit data stored in the detector
typedef DetectorData<geo_id_value, Acts::PlanarModuleCluster> DetData;

class StripSpacePointBuilder : public BareAlgorithm
{
public:

  /// @brief Configuration of the class to steer its behaviour
  struct Config
  {
    /// Input collection of digitized particles
    std::string collectionIn;
    /// Output collection of calculated hit coordinates
    std::string collectionOut;
  };

  /// Constructor
  /// @param cfg is the configuration class
  StripSpacePointBuilder(const Config&        cfg, Acts::Logging::Level level = Acts::Logging::INFO);

  /// @brief Executes the full algorithm. Extracts the data from Whiteboard,
  /// processes it and stores the space points on the Whiteboard.
  /// @param ctx is the Whiteboard that stores the data
  /// @return information is the algorithm was successful
  ProcessCode
  execute(AlgorithmContext ctx) const;

private:

	Config m_cfg;

  /// @brief Getter method for the digitized hits
  /// @param ctx is the reference to the Whiteboard that stores the data
  /// @param detData local pointer to all produced digitized hits
  /// @return information if the data was found or not
  ProcessCode
  clusterReading(AlgorithmContext& ctx, const DetData*& detData) const;
  
  void
  storeSpacePoint(const Acts::SingleHitSpacePoint& spacePoint, DetData& stripClusters) const;
  
  void
  storeSpacePoint(const Acts::DoubleHitSpacePoint& spacePoint, DetData& stripClusters) const;
    
  void
  writeData(const std::vector<Acts::SingleHitSpacePoint>& pixelSP, const std::vector<Acts::DoubleHitSpacePoint>& sctSP) const;
                                          
  void
  postProcess(DetData& clusters, const DetData*& detData) const;
};

}  // namespace FW

#endif  // ACTFW_ALGORITHMS_STRIPSPACEPOINTBUILDER_H
