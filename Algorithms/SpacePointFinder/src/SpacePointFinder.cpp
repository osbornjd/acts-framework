// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
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

const Acts::Vector2D
FW::SpacePointFinder::localCoords(const Acts::PlanarModuleCluster& hit) const
{
  // Local position information
  auto           par = hit.parameters();
  Acts::Vector2D local(par[Acts::ParDef::eLOC_0], par[Acts::ParDef::eLOC_1]);
  return local;
}

const Acts::Vector3D
FW::SpacePointFinder::globalCoords(const Acts::PlanarModuleCluster& hit) const
{
  // Receive corresponding surface
  auto& clusterSurface = hit.referenceSurface();

  // Transform local into global position information
  Acts::Vector3D pos, mom;
  clusterSurface.localToGlobal(localCoords(hit), mom, pos);

  return pos;
}

const double
FW::SpacePointFinder::differenceOfHits(
    const Acts::PlanarModuleCluster& hit1,
    const Acts::PlanarModuleCluster& hit2) const
{
  // Calculate the global position of the hits
  Acts::Vector3D pos1 = globalCoords(hit1);
  Acts::Vector3D pos2 = globalCoords(hit2);

  // Check if measurements are close enough to each other
  if (fabs(sqrt(pos1.x() * pos1.x() + pos1.y() * pos1.y() + pos1.z() * pos1.z())
           - sqrt(pos2.x() * pos2.x() + pos2.y() * pos2.y()
                  + pos2.z() * pos2.z()))
      > m_cfg.diffDist)
    return -1.;

  // Calculate the angles of the hits
  double phi1, theta1, phi2, theta2;
  phi1   = atan2(pos1.y() - m_cfg.vertex.y(), pos1.x() - m_cfg.vertex.x());
  theta1 = M_PI / 2
      - atan(pos1.z()
             - m_cfg.vertex.z() / sqrt((pos1.x() - m_cfg.vertex.x()) * pos1.x()
                                       - m_cfg.vertex.x())
             + (pos1.y() - m_cfg.vertex.x()) * (pos1.y() - m_cfg.vertex.x()));
  phi2   = atan2(pos2.y() - m_cfg.vertex.y(), pos2.x() - m_cfg.vertex.x());
  theta2 = M_PI / 2
      - atan(pos2.z()
             - m_cfg.vertex.z() / sqrt((pos2.x() - m_cfg.vertex.x()) * pos2.x()
                                       - m_cfg.vertex.x())
             + (pos2.y() - m_cfg.vertex.x()) * (pos2.y() - m_cfg.vertex.x()));

  // Calculate the squared difference between the theta angles
  double diffTheta2 = (theta1 - theta2) * (theta1 - theta2);
  if (diffTheta2 > m_cfg.diffTheta2) {
    ACTS_DEBUG("Squared theta angle "
               << diffTheta2
               << " too large - points are not combined");
    return -1.;
  }

  // Calculate the squared difference between the phi angles
  double diffPhi2 = (phi1 - phi2) * (phi1 - phi2);
  if (diffPhi2 > m_cfg.diffPhi2) {
    ACTS_DEBUG("Squared phi angle " << diffPhi2
                                    << " too large - points are not combined");
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
  std::vector<Acts::PlanarModuleCluster> const*
      module2;  // TODO: Annahme ist hier, dass es nur 2 surfaces pro layer gibt

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
          return;  // Muss evtl. angepasst werden
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
  for (auto& layer : allCombHits)
    // Walk through every hit combination of a layer
    for (auto& combination : layer) {
      for (auto& combinationCompare : layer)
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

const std::pair<Acts::Vector3D, Acts::Vector3D>
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
  auto& boundariesY = binData[1].boundaries();  // TODO: Es sollte erfahren
                                                // werden, wo die lange und die
                                                // kurze seite des strips ist

  // Search the x-/y-bin hit
  size_t binX = binData[0].searchLocal(local);
  size_t binY = binData[1].searchLocal(local);

  // Set the top and bottom end of the strip in local coordinates
  Acts::Vector2D topLocal = {(boundariesX[binX] + boundariesX[binX + 1]) / 2,
                             boundariesY[binY + 1]};
  Acts::Vector2D bottomLocal
      = {(boundariesX[binX] + boundariesX[binX + 1]) / 2, boundariesY[binY]};

  // Calculate the global coordinates of the top and bottom end of the strip
  Acts::Vector3D topGlobal, bottomGlobal, mom;  // mom is a dummy variable
  sur.localToGlobal(topLocal, mom, topGlobal);
  sur.localToGlobal(bottomLocal, mom, bottomGlobal);

  // Return the top and bottom end of the strip in global coordinates
  return std::make_pair(topGlobal, bottomGlobal);
}

std::unique_ptr<const std::vector<Acts::Vector3D>>
FW::SpacePointFinder::calculateSpacePoints(
    std::vector<std::vector<CombinedHits>>& allCombHits) const
{
  // Source of algorithm: Athena, SiSpacePointMakerTool::makeSCT_SpacePoint()

  // Declare storage of the found space points
  std::vector<Acts::Vector3D> spacePoints;

  // Walk over every found candidate pair
  for (auto& layers : allCombHits)
    for (auto& hits : layers) {
      // Calculate the ends of the strip detector elements
      auto& ends1 = endsOfStrip(*(hits.hitModule1));
      auto& ends2 = endsOfStrip(*(hits.hitModule2));

      /// The following algorithm is meant for finding the position on the first
      /// strip if there is a corresponding hit on the second strip. The
      /// resulting point is a point x on the first surfaces. This point is
      /// along
      /// a line between the points a (top end of the strip) and b (bottom end
      /// of the strip). The location can be parametrized as
      /// 	2 * x = (1 + m) a + (1 - m) b
      /// as function of the scalar m. m is a parameter in the interval
      /// -1 < m < 1 since the hit was on the strip. Furthermore, the vector
      /// from the vertex to the hit on the second strip y is needed to be a
      /// multiple k of the vector from vertex to the hit on the first strip x.
      /// As a consequence of this demand y = k * x needs to be on the
      /// connecting
      /// line between the top (c) and bottom (d) end of the second strip. If
      /// both hits correspond to each other, the condition
      /// 	y * (c X d) = k * x (c X d) = 0 ("X" represents a cross product)
      /// needs to be fulfilled. Inserting the first equation into this
      /// equation leads to the condition for m as given in the following
      /// algorithm
      /// and therefore to the calculation of x.
      Acts::Vector3D q  = ends1.first - ends1.second;
      Acts::Vector3D s  = ends1.first + ends1.second - 2 * m_cfg.vertex;
      Acts::Vector3D r  = ends2.first - ends2.second;
      Acts::Vector3D t  = ends2.first + ends2.second - 2 * m_cfg.vertex;
      Acts::Vector3D rt = r.cross(t);
      double         m  = -s.dot(rt) / q.dot(rt);

      // Check if hit is on strip
      if (fabs(m) > 1) {
        ACTS_INFO("Unable to combine hits with vertex");
      } else
        // Store the space point
        spacePoints.push_back(0.5 * (ends1.first + ends1.second + m * q));
    }
  // Return the resolved hits
  return std::make_unique<const std::vector<Acts::Vector3D>>(spacePoints);
}

FW::ProcessCode
FW::SpacePointFinder::execute(AlgorithmContext ctx) const
{
  // TODO:Check, ob es sich bei den Daten um strip Aufnahmen handelt. Vllt via
  // Abstaende oder Segmentation
  // TODO:Die Richtung aus der das Teilchen kommt ist fixed auf m_cfg.vertex.
  // Sollte vllt modifizierbar sein
  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);

  // DetData is typename of DetectorData<geo_id_value,
  // Acts::PlanarModuleCluster>
  const DetData*                                               detData;
  std::vector<std::vector<FW::SpacePointFinder::CombinedHits>> allCombHits;
  std::unique_ptr<const std::vector<Acts::Vector3D>>           spacePoints;

  // Receive all hits from the Whiteboard
  clusterReading(ctx, detData);

  // Extract hits that occur in opposite SCTs
  findOverlappingClusters(detData, allCombHits);

  // Filter entries
  filterCombinations(allCombHits);

  // Calculate the SpacePoints measured by the combination
  spacePoints = calculateSpacePoints(allCombHits);

  // Write to Whiteboard
  if (ctx.eventStore.add(m_cfg.collectionOut,
                         std::move(spacePoints))
      != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
