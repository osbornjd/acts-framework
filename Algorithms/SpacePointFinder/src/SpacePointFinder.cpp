// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/SpacePointFinder/SpacePointFinder.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>
#include "ACTFW/Framework/WhiteBoard.hpp"

FW::SpacePointFinder::SpacePointFinder(const FW::SpacePointFinder::Config& cfg,
                                       Acts::Logging::Level level)
  : FW::BareAlgorithm("SpacePointFinder", level), m_cfg(cfg)
{
  // Check that all mandatory configuration parameters are present
  if (m_cfg.collectionIn.empty()) {
    throw std::invalid_argument("Missing input collection");
  }

  if (m_cfg.collectionOut.empty()) {
    throw std::invalid_argument("Missing output collection");
  }

  // Print chosen configuration
  ACTS_DEBUG("Space Point Finder settings: ");
  ACTS_DEBUG("- intput collection: " << m_cfg.collectionIn);
  ACTS_DEBUG("- output collection: " << m_cfg.collectionOut);
  ACTS_DEBUG("- difference in squared theta: " << m_cfg.diffTheta2);
  ACTS_DEBUG("- difference in squared phi: " << m_cfg.diffPhi2);
  ACTS_DEBUG("- difference in distances: " << m_cfg.diffDist);
  ACTS_DEBUG("- vertex position: " << m_cfg.vertex);
}

FW::ProcessCode
FW::SpacePointFinder::clusterReading(AlgorithmContext& ctx,
                                     const DetData*&   detData) const
{
  // Load hit data from Whiteboard
  if (ctx.eventStore.get(m_cfg.collectionIn, detData)
      != FW::ProcessCode::SUCCESS) {
    ACTS_DEBUG("Unable to receive event data");
    return FW::ProcessCode::ABORT;
  }

  ACTS_DEBUG("Event data successfully received");
  return FW::ProcessCode::SUCCESS;
}

Acts::Vector2D
FW::SpacePointFinder::localCoords(const Acts::PlanarModuleCluster& hit) const
{
  // Local position information
  auto           par = hit.parameters();
  Acts::Vector2D local(par[Acts::ParDef::eLOC_0], par[Acts::ParDef::eLOC_1]);
  return local;
}

Acts::Vector3D
FW::SpacePointFinder::globalCoords(const Acts::PlanarModuleCluster& hit) const
{
  // Receive corresponding surface
  auto& clusterSurface = hit.referenceSurface();

  // Transform local into global position information
  Acts::Vector3D pos, mom;
  clusterSurface.localToGlobal(localCoords(hit), mom, pos);

  return pos;
}

double
FW::SpacePointFinder::differenceOfHits(
    const Acts::PlanarModuleCluster& hit1,
    const Acts::PlanarModuleCluster& hit2) const
{
  // Calculate the global position of the hits
  Acts::Vector3D pos1 = globalCoords(hit1);
  Acts::Vector3D pos2 = globalCoords(hit2);

  // Check if measurements are close enough to each other
  if ((pos1 - pos2).norm() > m_cfg.diffDist) return -1.;

  // Calculate the angles of the hits
  double phi1, theta1, phi2, theta2;
  phi1   = (pos1 - m_cfg.vertex).phi();
  theta1 = (pos1 - m_cfg.vertex).theta();
  phi2   = (pos2 - m_cfg.vertex).phi();
  theta2 = (pos2 - m_cfg.vertex).theta();

  // Calculate the squared difference between the theta angles
  double diffTheta2 = (theta1 - theta2) * (theta1 - theta2);
  if (diffTheta2 > m_cfg.diffTheta2) {
    ACTS_DEBUG(
        "Squared theta angle " << diffTheta2 << " between positions ("
                               << pos1.x()
                               << ", "
                               << pos1.y()
                               << ", "
                               << pos1.z()
                               << ") and ("
                               << pos2.x()
                               << ", "
                               << pos2.y()
                               << ", "
                               << pos2.z()
                               << ") too large - points are not combined");
    return -1.;
  }

  // Calculate the squared difference between the phi angles
  double diffPhi2 = (phi1 - phi2) * (phi1 - phi2);
  if (diffPhi2 > m_cfg.diffPhi2) {
    ACTS_DEBUG("Squared phi angle " << diffPhi2 << " between positions ("
                                    << pos1.x()
                                    << ", "
                                    << pos1.y()
                                    << ", "
                                    << pos1.z()
                                    << ") and ("
                                    << pos2.x()
                                    << ", "
                                    << pos2.y()
                                    << ", "
                                    << pos2.z()
                                    << ") too large - points are not combined");
    return -1.;
  }

  // Return the squared distance between both hits
  return diffTheta2 + diffPhi2;
}

std::vector<FW::SpacePointFinder::CombinedHits>
FW::SpacePointFinder::combineHits(
    const std::vector<Acts::PlanarModuleCluster>& vec1,
    const std::vector<Acts::PlanarModuleCluster>& vec2) const
{
  // TODO: only the closest differences get selected -> some points are not
  // taken into account
  // Declare helper variables
  double                                          currentDiff;
  FW::SpacePointFinder::CombinedHits              tmpCombHits;
  std::vector<FW::SpacePointFinder::CombinedHits> combHits;
  double                                          diffMin;
  unsigned int                                    hitMin;

  // Walk through all hits on both surfaces
  for (unsigned int iVec1 = 0; iVec1 < vec1.size(); iVec1++) {
    // Set the closest distance to the maximum of double
    diffMin = std::numeric_limits<double>::max();
    // Set the corresponding index to an element not in the list of hits
    hitMin = vec2.size();
    for (unsigned int iVec2 = 0; iVec2 < vec2.size(); iVec2++) {
      // Calculate the distances between the hits
      currentDiff = differenceOfHits(vec1[iVec1], vec2[iVec2]);
      // Store the closest hits (distance and index) calculated so far
      if (currentDiff < diffMin && currentDiff >= 0.) {
        diffMin = currentDiff;
        hitMin  = iVec2;
      }
    }
    // Store the best (=closest) result
    if (hitMin < vec2.size()) {
      tmpCombHits.hitModule1 = &(vec1[iVec1]);
      tmpCombHits.hitModule2 = &(vec2[hitMin]);
      tmpCombHits.diff       = diffMin;
      combHits.push_back(tmpCombHits);
    }
  }

  // Return the list of closest hits
  return combHits;
}

void
FW::SpacePointFinder::findOverlappingClusters(
    const DetData*&                                               detData,
    std::vector<std::vector<FW::SpacePointFinder::CombinedHits>>& allCombHits)
    const
{
  // Declare temporary storage
  std::vector<Acts::PlanarModuleCluster> const* module1;
  std::vector<Acts::PlanarModuleCluster> const* module2;
  // TODO: Only treats two surfaces per layer

  // Loop over the planar clusters in this event
  for (auto& volumeData : *detData)
    for (auto& layerData : volumeData.second) {
      // Set to nullptr as veto
      module1 = nullptr;
      module2 = nullptr;

      for (auto& moduleData : layerData.second) {
        // Store the Hits of the 1st and 2nd surface
        if (moduleData.first == 1)
          module1 = &moduleData.second;
        else if (moduleData.first == 2)
          module2 = &moduleData.second;
        else {
          ACTS_INFO("Warning: More than two Surfaces not implemented yet");
          return;
        }
      }
      // Get the combination of hits
      if (module1 && module2) {
        std::vector<FW::SpacePointFinder::CombinedHits> combHits
            = combineHits(*module1, *module2);
        allCombHits.push_back(std::move(combHits));
      } else
        ACTS_VERBOSE("Layer " << layerData.first << " stored no surfaces");
    }
}

void
FW::SpacePointFinder::filterCombinations(
    std::vector<std::vector<FW::SpacePointFinder::CombinedHits>>& allCombHits)
    const
{
  // Walk through the layers
  for (const auto& layer : allCombHits)
    // Walk through every hit combination of a layer
    for (const auto& combination : layer) {
      for (const auto& combinationCompare : layer)
        // Check, if combinations infere and resolve the problem.
        // Hits on the first module are different from each other by
        // construction but a hit on the second module can appear in multiple
        // combinations.
        if ((combination.hitModule1 != combinationCompare.hitModule1)
            && (combination.hitModule2 == combinationCompare.hitModule2))
          ACTS_INFO(
              "Warning: Multiple candidate selection not implemented yet");
    }
}

std::pair<Acts::Vector3D, Acts::Vector3D>
FW::SpacePointFinder::endsOfStrip(const Acts::PlanarModuleCluster& hit) const
{
  // Calculate the local coordinates of the hit
  const Acts::Vector2D local = localCoords(hit);

  // Receive the binning
  auto& sur        = hit.referenceSurface();
  auto  genDetElem = dynamic_cast<const FWGen::GenericDetectorElement*>(
      sur.associatedDetectorElement());
  auto segment = dynamic_cast<const Acts::CartesianSegmentation*>(
      &(genDetElem->digitizationModule()->segmentation()));
  auto& binData     = segment->binUtility().binningData();
  auto& boundariesX = binData[0].boundaries();
  auto& boundariesY = binData[1].boundaries();

  // Search the x-/y-bin hit
  size_t binX = binData[0].searchLocal(local);
  size_t binY = binData[1].searchLocal(local);

  Acts::Vector2D topLocal, bottomLocal;

  if (boundariesX[binX + 1] - boundariesX[binX]
      < boundariesY[binY + 1] - boundariesY[binY]) {
    // Set the top and bottom end of the strip in local coordinates
    topLocal = {(boundariesX[binX] + boundariesX[binX + 1]) / 2,
                boundariesY[binY + 1]};
    bottomLocal
        = {(boundariesX[binX] + boundariesX[binX + 1]) / 2, boundariesY[binY]};
  } else {
    // Set the top and bottom end of the strip in local coordinates
    topLocal
        = {boundariesX[binX], (boundariesY[binY] + boundariesY[binY + 1]) / 2};
    bottomLocal = {boundariesX[binX + 1],
                   (boundariesY[binY] + boundariesY[binY + 1]) / 2};
  }

  // Calculate the global coordinates of the top and bottom end of the strip
  Acts::Vector3D topGlobal, bottomGlobal, mom;  // mom is a dummy variable
  sur.localToGlobal(topLocal, mom, topGlobal);
  sur.localToGlobal(bottomLocal, mom, bottomGlobal);

  // Return the top and bottom end of the strip in global coordinates
  return std::make_pair(topGlobal, bottomGlobal);
}

//~ std::unique_ptr<const std::vector<Acts::Vector3D>>
FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>
FW::SpacePointFinder::calculateSpacePoints(
    std::vector<std::vector<CombinedHits>>& allCombHits,
    const DetData*&                         detData) const
{
  // Source of algorithm: Athena, SiSpacePointMakerTool::makeSCT_SpacePoint()
  // TODO: some stability part still missing

  // Declare storage of the found space points
  std::vector<Acts::Vector3D> spacePoints;

  FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster> stripClusters;

  // Walk over every found candidate pair
  for (const auto& layers : allCombHits)
    for (const auto& hits : layers) {
      // Calculate the ends of the strip detector elements
      const auto& ends1 = endsOfStrip(*(hits.hitModule1));
      const auto& ends2 = endsOfStrip(*(hits.hitModule2));

      /// The following algorithm is meant for finding the position on the first
      /// strip if there is a corresponding hit on the second strip. The
      /// resulting point is a point x on the first surfaces. This point is
      /// along a line between the points a (top end of the strip)
      /// and b (bottom end of the strip). The location can be parametrized as
      /// 	2 * x = (1 + m) a + (1 - m) b
      /// as function of the scalar m. m is a parameter in the interval
      /// -1 < m < 1 since the hit was on the strip. Furthermore, the vector
      /// from the vertex to the hit on the second strip y is needed to be a
      /// multiple k of the vector from vertex to the hit on the first strip x.
      /// As a consequence of this demand y = k * x needs to be on the
      /// connecting line between the top (c) and bottom (d) end of
      /// the second strip. If both hits correspond to each other, the condition
      /// 	y * (c X d) = k * x (c X d) = 0 ("X" represents a cross product)
      /// needs to be fulfilled. Inserting the first equation into this
      /// equation leads to the condition for m as given in the following
      /// algorithm and therefore to the calculation of x.

      Acts::Vector3D q  = ends1.first - ends1.second;
      Acts::Vector3D s  = ends1.first + ends1.second - 2 * m_cfg.vertex;
      Acts::Vector3D r  = ends2.first - ends2.second;
      Acts::Vector3D t  = ends2.first + ends2.second - 2 * m_cfg.vertex;
      Acts::Vector3D rt = r.cross(t);
      double         m  = -s.dot(rt) / q.dot(rt);

      // Check if hit is on strip
      if (fabs(m) > 1) {
        ACTS_INFO("Unable to combine hits with vertex (" << m_cfg.vertex[0]
                                                         << ", "
                                                         << m_cfg.vertex[1]
                                                         << ", "
                                                         << m_cfg.vertex[2]
                                                         << ")");
      } else {
        // Store the space point
        spacePoints.push_back(0.5 * (ends1.first + ends1.second + m * q));

        // Receive the identification of the digitized hits on the first surface
        Identifier       id(hits.hitModule1->identifier());
        Acts::GeometryID geoID(id.value());

        // The covariance is currently set to 0.
        Acts::ActsSymMatrixD<2> cov;
        cov << 0., 0., 0., 0.;

        // Get the local coordinates of the space point
        Acts::Vector2D local;
        hits.hitModule1->referenceSurface().globalToLocal(
            spacePoints.back(), {0., 0., 0.}, local);

        // Build the space point
        Acts::PlanarModuleCluster pCluster(
            hits.hitModule1->referenceSurface(),
            Identifier(geoID.value()),
            std::move(cov),
            local[0],
            local[1],
            std::move(hits.hitModule1->digitizationCells()),
            {hits.hitModule1->truthVertices()});

        // Insert into the cluster map
        FW::Data::insert(stripClusters,
                         geoID.value(Acts::GeometryID::volume_mask),
                         geoID.value(Acts::GeometryID::layer_mask),
                         geoID.value(Acts::GeometryID::sensitive_mask),
                         std::move(pCluster));
      }
    }
  // Return the resolved hits
  return stripClusters;
}

FW::ProcessCode
FW::SpacePointFinder::execute(AlgorithmContext ctx) const
{
  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);

  // DetData is typename of DetectorData<geo_id_value,
  // Acts::PlanarModuleCluster>
  const DetData*                                               detData;
  std::vector<std::vector<FW::SpacePointFinder::CombinedHits>> allCombHits;
  FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster> stripClusters;

  // Receive all hits from the Whiteboard
  clusterReading(ctx, detData);

  // Extract hits that occur in opposite SCTs
  findOverlappingClusters(detData, allCombHits);

  // Filter entries
  //~ filterCombinations(allCombHits);

  // Calculate the SpacePoints measured by the combination
  stripClusters = calculateSpacePoints(allCombHits, detData);

  // Write to Whiteboard
  if (ctx.eventStore.add(m_cfg.collectionOut, std::move(stripClusters))
      != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
