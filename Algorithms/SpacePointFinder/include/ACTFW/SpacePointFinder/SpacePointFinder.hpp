// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_ALGORITHMS_SPACEPOINTFINDER_H
#define ACTFW_ALGORITHMS_SPACEPOINTFINDER_H

#include <ACTS/Digitization/PlanarModuleCluster.hpp>
#include <ACTS/Utilities/Units.hpp>
#include <array>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"
#include "ACTS/Digitization/CartesianSegmentation.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// Type of the hit data stored in the detector
typedef DetectorData<geo_id_value, Acts::PlanarModuleCluster> DetData;

/// @class SpacePointFinder
///
/// After the particle interaction with surfaces are recorded and digitized
/// the hits strip detectors need further treatment. This class takes
/// the digitized hits and combines them on at least two different
/// strips to a result of the combined detector element.
///

class SpacePointFinder : public BareAlgorithm
{
public:
  struct Config
  {
    /// Input collection of digitized particles
    std::string collectionIn;
    /// Output collection of calculated hit coordinates
    std::string collectionOut;
    /// Accepted difference in eta for two hits
    double diffTheta2 = 1.;
    /// Accepted difference in phi for two hits
    double diffPhi2 = 1.;
    /// Accepted distance between two hits
    double diffDist = 100.;
    /// Assumed position of the vertex
    Acts::Vector3D vertex = {0., 0., 0.};
  };

  /// Structure for easier bookkeeping of potential hit combinations
  /// on two surfaces
  struct CombinedHits
  {
    /// Storage of the hit on the first surface
    Acts::PlanarModuleCluster const* hitModule1;
    /// Storage of the hit on the second surface
    Acts::PlanarModuleCluster const* hitModule2;
    /// Storage of (Delta eta)^2 + (Delta phi)^2
    /// Allows vetos between different possible combinations
    double diff;
  };

  /// Constructor
  /// @param cfg is the configuration class
  SpacePointFinder(const Config&        cfg,
                   Acts::Logging::Level level = Acts::Logging::INFO);

  /// Executes the full algorithm. Extracts the data from Whiteboard,
  /// processes it and stores the space points on the Whiteboard.
  /// @param ctx is the Whiteboard that stores the data
  /// @return information is the algorithm was successful
  ProcessCode
  execute(AlgorithmContext ctx) const;

private:
  Config m_cfg;

  /// Getter method for the digitized hits
  /// @param ctx is the reference to the Whiteboard that stores the data
  /// @param detData local pointer to all produced digitized hits
  /// @return information if the data was found or not
  ProcessCode
  clusterReading(AlgorithmContext& ctx, const DetData*& detData) const;

  /// Getter method for the local coordinates of a hit
  /// on its corresponding surface
  /// @param hit object related to the hit that holds the necessary information
  /// @return vector of the local coordinates of the hit on the surface
  const Acts::Vector2D
  localCoords(const Acts::PlanarModuleCluster& hit) const;

  /// Getter method for the global coordinates of a hit
  /// @param hit object related to the hit that holds the necessary information
  /// @return vector of the global coordinates of the hit
  const Acts::Vector3D
  globalCoords(const Acts::PlanarModuleCluster& hit) const;

  /// Calculates (Delta theta)^2 + (Delta phi)^2 between two hits
  /// @param hit1 the first hit
  /// @param hit2 the second hit
  /// @return the squared sum in case of success, otherwise -1
  const double
  differenceOfHits(const Acts::PlanarModuleCluster& hit1,
                   const Acts::PlanarModuleCluster& hit2) const;

  /// Searches possible combinations of two hits on different surfaces
  /// that may come from the same particles
  /// @param vec1 vector of hits on the first surface
  /// @param vec2 vector of hits on the second surface
  /// @return structure that stores all candidates of combined hits
  std::vector<CombinedHits>
  combineHits(const std::vector<Acts::PlanarModuleCluster>& vec1,
              const std::vector<Acts::PlanarModuleCluster>& vec2) const;

  /// Searches and stores all hit combination candidates in the detector
  /// @param detData local pointer to all produced digitized hits
  /// @param allCombHits matrix that stores all hit combination candidates
  void
  findOverlappingClusters(
      const DetData*&                         detData,
      std::vector<std::vector<CombinedHits>>& allCombHits) const;

  /// No function, still TODO
  void
  filterCombinations(std::vector<std::vector<CombinedHits>>& allCombHits) const;

  /// Calculates the top and bottom ends of a strip detector element
  /// that corresponds to a given hit
  /// @param hit object that stores the information about the hit
  /// @return vectors to the top and bottom end of the strip detector element
  const std::pair<Acts::Vector3D, Acts::Vector3D>
  endsOfStrip(const Acts::PlanarModuleCluster& hit) const;

  /// Calculates the space points out of a given collection of hits
  /// on several strip detectors
  /// @param allCombHits matrix that stores all hit combination candidates
  /// @return pointer to all resolved space points
  std::unique_ptr<const std::vector<Acts::Vector3D>>
  calculateSpacePoints(
      std::vector<std::vector<CombinedHits>>& allCombHits) const;
};

}  // namespace FW

#endif  // ACTFW_ALGORITHMS_SPACEPOINTFINDER_H
