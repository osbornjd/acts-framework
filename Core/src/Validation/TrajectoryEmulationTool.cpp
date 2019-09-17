// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Validation/TrajectoryEmulationTool.hpp"

FW::TrajectoryEmulationTool::TrajectoryEmulationTool(
    const FW::TrajectoryEmulationTool::Config& cfg,
    Acts::Logging::Level                       level)
  : m_cfg(cfg)
  , m_logger(Acts::getDefaultLogger("TrajectoryEmulationTool", level))
{
  if (!m_cfg.trajectoryMultiplicity) {
    throw std::invalid_argument(
        "No generator for trajectory multiplicity is configured!");
  }
  if (!m_cfg.outlierMultiplicity) {
    throw std::invalid_argument(
        "No generator for emulated outlier multiplicity is configured!");
  }
  if (!m_cfg.holeMultiplicity) {
    throw std::invalid_argument(
        "No generator for emulated hole multiplicity is configured!");
  }
  if (!m_cfg.randomNumbers) {
    throw std::invalid_argument("Missing random numbers service");
  }
}

FW::TrajectoryEmulationTool::SourceLinksVector
FW::TrajectoryEmulationTool::emulate(
    const AlgorithmContext&                         ctx,
    const FW::TrajectoryEmulationTool::SourceLinks& truthSourceLinks) const
{
  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);

  // lambda to create outliers on a trajectory
  auto createOutlier =
      [&](const FW::TrajectoryEmulationTool::SourceLinks& sLinks,
          const std::vector<int>                          outlierIndexs) {
        FW::TrajectoryEmulationTool::SourceLinks emulatedTraj = {};
        for (size_t iSL = 0; iSL < sLinks.size(); iSL++) {
          bool isOutlier = false;
          for (size_t iOutlier = 0; iOutlier < outlierIndexs.size();
               iOutlier++) {
            if (outlierIndexs[iOutlier] == iSL) {
              isOutlier = true;
              break;
            }
          }
          if (isOutlier) {
            auto& hit        = sLinks[iSL].truthHit();
            auto  hitSurface = hit.surface;
            // get the geoID and transform of the surface
            auto geoID = hitSurface->geoID();

            // transform global into local position
            Acts::Vector2D local(0, 0);
            Acts::Vector3D mom(1, 1, 1);
            hitSurface->globalToLocal(ctx.geoContext, hit.position, mom, local);

            // smear the truth hit and set the covariance
            //@todo: random offset
            double                  resX = 30 * Acts::units::_um;
            double                  resY = 30 * Acts::units::_um;
            Acts::ActsSymMatrixD<2> cov2D;
            cov2D << resX * resX, 0., 0., resY * resY;

            // smear is based on 10*resolution
            double dx = resX * 10;
            double dy = resY * 10;

            // move a ,LOC_0, LOC_1 measurement
            // make source link with smeared hit

            Acts::BoundMatrix cov{};
            cov.topLeftCorner<2, 2>() = cov2D;

            Acts::BoundVector loc{};
            loc.head<2>() << local[Acts::ParDef::eLOC_0] + dx,
                local[Acts::ParDef::eLOC_1] + dy;

            // create an outlier
            Data::SimSourceLink outlier{&hit, 2, loc, cov};
            emulatedTraj.push_back(outlier);
          } else {
            emulatedTraj.push_back(sLinks[iSL]);
          }
        }
        return emulatedTraj;
      };

  // lambda to create holes on a trajectory
  auto createHole = [](const FW::TrajectoryEmulationTool::SourceLinks& sLinks,
                       const std::vector<int> holeIndexs) {
    FW::TrajectoryEmulationTool::SourceLinks emulatedTraj = {};
    for (size_t iSL = 0; iSL < sLinks.size(); iSL++) {
      bool keep = true;
      for (size_t iHole = 0; iHole < holeIndexs.size(); iHole++) {
        if (holeIndexs[iHole] == iSL) {
          keep = false;
          break;
        }
      }
      if (keep) { emulatedTraj.push_back(sLinks[iSL]); }
    }
    return emulatedTraj;
  };

  // Random number for the locations of the outliers or holes
  std::uniform_int_distribution<> fakeHitLocation(0,
                                                  truthSourceLinks.size() - 1);

  FW::TrajectoryEmulationTool::SourceLinksVector emulatedTrajectories = {};
  // create trajectories with outliers and holes
  for (size_t iTraj = m_cfg.trajectoryMultiplicity(rng); 0 < iTraj; --iTraj) {
    // 1) determine number of outliers and holes
    size_t attemptedNumOutliers = m_cfg.outlierMultiplicity(rng);
    size_t numOutliers          = attemptedNumOutliers < truthSourceLinks.size()
        ? attemptedNumOutliers
        : 0;
    if (numOutliers == 0) continue;
    size_t attemptedNumHoles = m_cfg.holeMultiplicity(rng);
    size_t numHoles
        = attemptedNumHoles < truthSourceLinks.size() ? attemptedNumHoles : 0;
    if (numHoles == 0) continue;

    // 2) determine the locations of the outliers and holes
    // vector to store the index of outliers and holes
    std::vector<int> outlierIndexs = {}, holeIndexs = {};
    for (size_t iOutlier = 0; iOutlier < numOutliers; iOutlier++) {
      outlierIndexs.push_back(fakeHitLocation(rng));
    }
    for (size_t iHole = 0; iHole < numHoles; iHole++) {
      holeIndexs.push_back(fakeHitLocation(rng));
    }

    // 3) then create a trajectory with outliers and holes
    auto outlierTraj = createOutlier(truthSourceLinks, outlierIndexs);
    auto traj        = createHole(outlierTraj, holeIndexs);

    // 4) push the trajectory
    emulatedTrajectories.push_back(traj);
  }
  return emulatedTrajectories;
}
