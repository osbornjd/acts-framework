// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Io/Csv/CsvPlanarClusterWriter.hpp"

#include <stdexcept>

#include <Acts/Plugins/Digitization/PlanarModuleCluster.hpp>
#include <Acts/Utilities/Units.hpp>
#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "TrackMlData.hpp"

FW::CsvPlanarClusterWriter::CsvPlanarClusterWriter(
    const FW::CsvPlanarClusterWriter::Config& cfg,
    Acts::Logging::Level                      lvl)
  : WriterT(cfg.inputClusters, "CsvPlanarClusterWriter", lvl), m_cfg(cfg)
{
  // inputClusters is already checked by base constructor
}

FW::ProcessCode
FW::CsvPlanarClusterWriter::writeT(
    const AlgorithmContext&                                  ctx,
    const FW::GeometryIdMultimap<Acts::PlanarModuleCluster>& clusters)
{
  // open per-event file for all components
  std::string pathHits
      = perEventFilepath(m_cfg.outputDir, "hits.csv", ctx.eventNumber);
  std::string pathCells
      = perEventFilepath(m_cfg.outputDir, "cells.csv", ctx.eventNumber);
  std::string pathTruth
      = perEventFilepath(m_cfg.outputDir, "truth.csv", ctx.eventNumber);

  dfe::NamedTupleCsvWriter<HitData> writerHits(pathHits, m_cfg.outputPrecision);
  dfe::NamedTupleCsvWriter<CellData>     writerCells(pathCells,
                                                 m_cfg.outputPrecision);
  dfe::NamedTupleCsvWriter<TruthHitData> writerTruth(pathTruth,
                                                     m_cfg.outputPrecision);

  HitData      hit;
  CellData     cell;
  TruthHitData truth;
  // will be reused as hit counter
  hit.hit_id = 0;

  for (const auto& entry : clusters) {
    Acts::GeometryID                 geoId   = entry.first;
    const Acts::PlanarModuleCluster& cluster = entry.second;
    // local cluster information
    const auto&    parameters = cluster.parameters();
    Acts::Vector2D localPos(parameters[0], parameters[1]);
    Acts::Vector3D globalFakeMom(1, 1, 1);
    Acts::Vector3D globalPos(0, 0, 0);
    // transform local into global position information
    cluster.referenceSurface().localToGlobal(
        ctx.geoContext, localPos, globalFakeMom, globalPos);

    // encoded geometry identifier
    hit.geometry_id = geoId.value();
    // (partially) decoded geometry identifier
    hit.volume_id = geoId.volume();
    hit.layer_id  = geoId.layer();
    hit.module_id = geoId.sensitive();
    // write global hit information
    hit.x = globalPos.x() / Acts::UnitConstants::mm;
    hit.y = globalPos.y() / Acts::UnitConstants::mm;
    hit.z = globalPos.z() / Acts::UnitConstants::mm;
    hit.t = parameters[2] / Acts::UnitConstants::ns;
    writerHits.append(hit);

    // write local cell information
    cell.hit_id = hit.hit_id;
    for (auto& c : cluster.digitizationCells()) {
      cell.ch0 = c.channel0;
      cell.ch1 = c.channel1;
      // TODO store digitial timestamp once added to the cell definition
      cell.timestamp = 0;
      cell.value     = c.data;
      writerCells.append(cell);
    }

    // write hit-particle truth association
    // each hit can have multiple particles, e.g. in a dense environment
    truth.hit_id      = hit.hit_id;
    truth.geometry_id = hit.geometry_id;
    for (auto& p : cluster.sourceLink().truthParticles()) {
      truth.particle_id = p->barcode().value();
      truth.tx          = p->position().x() / Acts::UnitConstants::mm;
      truth.ty          = p->position().y() / Acts::UnitConstants::mm;
      truth.tz          = p->position().z() / Acts::UnitConstants::mm;
      truth.tt          = p->time() / Acts::UnitConstants::ns;
      truth.tpx         = p->momentum().x() / Acts::UnitConstants::GeV;
      truth.tpy         = p->momentum().y() / Acts::UnitConstants::GeV;
      truth.tpz         = p->momentum().z() / Acts::UnitConstants::GeV;
      truth.te          = p->E() / Acts::UnitConstants::GeV;
      writerTruth.append(truth);
    }

    // increase hit id for next iteration
    hit.hit_id += 1;
  }

  return FW::ProcessCode::SUCCESS;
}
