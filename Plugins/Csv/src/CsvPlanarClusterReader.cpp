// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"

#include <fstream>
#include <ios>
#include <stdexcept>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvReader.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"
#include "Acts/Plugins/Identification/IdentifiedDetectorElement.hpp"

FW::Csv::CsvPlanarClusterReader::CsvPlanarClusterReader(
    const FW::Csv::CsvPlanarClusterReader::Config& cfg,
    Acts::Logging::Level                           level)
  : m_cfg(cfg)
  // TODO check that all files (hits,cells,truth) exists
  , m_numEvents(determineEventFilesRange(cfg.inputDir, "hits.csv").second)
  , m_logger(Acts::getDefaultLogger("CsvPlanarClusterReader", level))
{
  if (not m_cfg.trackingGeometry) {
    throw std::invalid_argument("Missing tracking geometry");
  }
  if (m_cfg.output.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
}

std::string
FW::Csv::CsvPlanarClusterReader::CsvPlanarClusterReader::name() const
{
  return "CsvPlanarClusterReader";
}

size_t
FW::Csv::CsvPlanarClusterReader::CsvPlanarClusterReader::numEvents() const
{
  return m_numEvents;
}

FW::ProcessCode
FW::Csv::CsvPlanarClusterReader::read(const FW::AlgorithmContext& ctx)
{
  // Prepare the output data: Clusters
  FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster> planarClusters;

  // open per-event hits .csv file with ',' as delimeter
  std::string pathHits
      = perEventFilepath(m_cfg.inputDir, "hits.csv", ctx.eventNumber);
  FW::CsvReader hitCsvReader(pathHits);

  // open per-event hit details .csv file with ',' as delimeter
  std::string pathDetails
      = perEventFilepath(m_cfg.inputDir, "cells.csv", ctx.eventNumber);
  FW::CsvReader detailCsvReader(pathDetails);

  // open per-event truth .csv file with ',' as delimeter
  std::string pathTruth
      = perEventFilepath(m_cfg.inputDir, "truth.csv", ctx.eventNumber);
  FW::CsvReader truthCsvReader(pathTruth);

  if (hitCsvReader.numPars() < 7) {
    ACTS_ERROR("Number of csv parameters in file '"
               << pathHits << "' needs to be at least 7 in the order: "
               << "'hit_id,x,y,z,volume_id,layer_id,module_id'"
               << ". Aborting. ");
    return FW::ProcessCode::ABORT;
  }

  if (detailCsvReader.numPars() < 4) {
    ACTS_ERROR("Number of csv parameters in file '"
               << pathDetails << "' needs to be at least 4 in the order: "
               << "'hit_id,ch0,ch1,value'"
               << ". Aborting. ");
    return FW::ProcessCode::ABORT;
  }

  if (truthCsvReader.numPars() < 8) {
    ACTS_ERROR("Number of csv parameters in file '"
               << pathTruth << "' needs to be at least 8 in the order: "
               << "'hit_id,particle_id,tx,ty,tz,tpx,tpy,tpz'"
               << ". Aborting. ");
    return FW::ProcessCode::ABORT;
  }

  std::vector<std::string> hitVal;
  std::vector<std::string> detailVal;
  std::vector<std::string> truthVal;

  while (hitCsvReader.readLine(hitVal)) {
    auto hit_id    = std::stoul(hitVal[0]);
    auto volumeKey = std::stoul(hitVal[4]);
    auto layerKey  = std::stoul(hitVal[5]);
    auto moduleKey = std::stoul(hitVal[6]);

    // create the GeometryID
    Acts::GeometryID geoID(0);
    geoID.add(volumeKey, Acts::GeometryID::volume_mask);
    geoID.add(layerKey, Acts::GeometryID::layer_mask);
    geoID.add(moduleKey, Acts::GeometryID::sensitive_mask);

    // retrieve the surface via geoID
    const Acts::Surface* hitSurface = nullptr;
    m_cfg.trackingGeometry->visitSurfaces(
        [&hitSurface, &geoID](const Acts::Surface* srf) {
          if (srf->geoID().value() == geoID.value()) hitSurface = srf;
        });
    if (!hitSurface) {
      ACTS_ERROR("Could not retrieve the surface with geoID = "
                 << geoID.value() << ". Skipping hit with hit_id = " << hit_id);
      continue;
    }

    // transform global into local position
    Acts::Vector3D pos(
        std::stof(hitVal[1]), std::stof(hitVal[2]), std::stof(hitVal[3]));
    Acts::Vector2D local(0, 0);
    Acts::Vector3D mom(1, 1, 1);
    hitSurface->globalToLocal(ctx.geoContext, pos, mom, local);

    Acts::ActsSymMatrixD<2> cov;
    cov << 0., 0., 0., 0.;

    // get digitization cells association
    std::vector<Acts::DigitizationCell> dCells;
    // retrieve the hit detail with the same hit_id
    while (detailCsvReader.peekLine().size() != 0
           && std::stoul(detailCsvReader.peekLine()[0])
               == std::stoul(hitVal[0])) {
      if (detailCsvReader.readLine(detailVal)) {
        dCells.push_back(Acts::DigitizationCell(std::stoul(detailVal[1]),
                                                std::stoul(detailVal[2]),
                                                std::stof(detailVal[3])));
      }
    }

    // get hit-particle truth association
    std::vector<const FW::Data::SimParticle*> hitParticles;
    // retrieve the hit detail with the same hit_id
    while (truthCsvReader.peekLine().size() != 0
           && std::stoul(truthCsvReader.peekLine()[0])
               == std::stoul(hitVal[0])) {
      if (truthCsvReader.readLine(truthVal)) {
        Acts::Vector3D sPosition(std::stof(truthVal[2]),
                                 std::stof(truthVal[3]),
                                 std::stof(truthVal[4]));
        Acts::Vector3D sMomentum(std::stof(truthVal[5]),
                                 std::stof(truthVal[6]),
                                 std::stof(truthVal[7]));

        barcode_type barcode = std::stoul(truthVal[1]);

        //@TODO: get q, mass and pdg from config?
        double   q    = 0;
        double   mass = 0.;
        pdg_type pdg  = 0;

        ACTS_VERBOSE("particle barcode = "
                     << barcode << " : position = (" << sPosition[0] << ", "
                     << sPosition[1] << ", " << sPosition[2] << ")");

        // creat a truth particle
        FW::Data::SimParticle* sParticle = new FW::Data::SimParticle(
            sPosition, sMomentum, mass, q, pdg, barcode);
        hitParticles.push_back(sParticle);
      }
    }

    ACTS_VERBOSE("hit_id = "
                 << hit_id << " : geoID = " << geoID << " : globalPos = ("
                 << pos[0] << ", " << pos[1] << ", " << pos[2] << ")"
                 << " : localPos = (" << local[0] << ", " << local[1] << ")"
                 << " : nCells = " << dCells.size()
                 << " : nTruthParticles = " << hitParticles.size());

    // create the planar cluster
    Acts::PlanarModuleCluster pCluster(
        hitSurface->getSharedPtr(),
        Identifier(Identifier::identifier_type(geoID.value()), hitParticles),
        std::move(cov),
        local[0],
        local[1],
        std::move(dCells));

    // insert the cluster into the cluster map
    FW::Data::insert(
        planarClusters, volumeKey, layerKey, moduleKey, std::move(pCluster));
  }

  // write the clusters to the EventStore
  ctx.eventStore.add(m_cfg.output, std::move(planarClusters));

  return FW::ProcessCode::SUCCESS;
}
