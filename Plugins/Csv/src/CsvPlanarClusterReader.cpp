// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"

#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/HitContainers.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTFW/Utilities/Range.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"
#include "Acts/Plugins/Identification/IdentifiedDetectorElement.hpp"
#include "TrackMlData.hpp"

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
  if (m_cfg.outputClusters.empty()) {
    throw std::invalid_argument("Missing cluster output collection");
  }
  if (m_cfg.outputHitParticleMap.empty()) {
    throw std::invalid_argument("Missing hit-particle map output collection");
  }
  // fill the geo id to surface map once to speed up lookups later on
  m_cfg.trackingGeometry->visitSurfaces([this](const Acts::Surface* surface) {
    this->m_surfaces[surface->geoID()] = surface;
  });
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

namespace {
struct HitIdComparator
{
  template <typename T>
  bool
  operator()(const T& left, const T& right)
  {
    return left.hit_id < right.hit_id;
  }
  template <typename T>
  bool
  operator()(uint64_t left_id, const T& right)
  {
    return left_id < right.hit_id;
  }
  template <typename T>
  bool
  operator()(const T& left, uint64_t right_id)
  {
    return left.hit_id < right_id;
  }
};
}  // namespace

FW::ProcessCode
FW::Csv::CsvPlanarClusterReader::read(const FW::AlgorithmContext& ctx)
{
  // Prepare the output data
  DetectorData<geo_id_value, Acts::PlanarModuleCluster> clusters;
  HitParticleMap                                        hitParticleMap;

  // per-event file paths
  std::string pathTruth
      = perEventFilepath(m_cfg.inputDir, "truth.csv", ctx.eventNumber);
  std::string pathCells
      = perEventFilepath(m_cfg.inputDir, "cells.csv", ctx.eventNumber);
  std::string pathHits
      = perEventFilepath(m_cfg.inputDir, "hits.csv", ctx.eventNumber);
  // open readers
  dfe::CsvNamedTupleReader<TruthData> truthReader(pathTruth);
  dfe::CsvNamedTupleReader<CellData>  cellReader(pathCells);
  dfe::CsvNamedTupleReader<HitData>   hitReader(pathHits);

  // since a hit
  // read all hits first
  std::vector<TruthData> truths;
  {
    TruthData truth;
    while (truthReader.read(truth)) {
      truths.push_back(truth);
      // hit ids should be increasing monotonically
      hitParticleMap.emplace_hint(
          hitParticleMap.end(), truth.hit_id, truth.particle_id);
    }
    std::sort(truths.begin(), truths.end(), HitIdComparator{});
  }
  std::vector<CellData> cells;
  {
    CellData cell;
    while (cellReader.read(cell)) { cells.push_back(cell); }
    std::sort(cells.begin(), cells.end(), HitIdComparator{});
  }

  // read the actual hit information
  HitData hit;
  while (hitReader.read(hit)) {

    Acts::GeometryID geoID;
    geoID.add(hit.volume_id, Acts::GeometryID::volume_mask);
    geoID.add(hit.layer_id, Acts::GeometryID::layer_mask);
    geoID.add(hit.module_id, Acts::GeometryID::sensitive_mask);

    // identify corresponding surface
    auto it = m_surfaces.find(geoID);
    if (it == m_surfaces.end() or not it->second) {
      ACTS_ERROR("Could not retrieve the surface for hit " << hit);
      continue;
    }
    const Acts::Surface& surface = *(it->second);

    // find matching truth particle information
    // TODO who owns these particles?
    std::vector<const FW::Data::SimParticle*> particles;
    {
      auto range = makeRange(std::equal_range(
          truths.begin(), truths.end(), hit.hit_id, HitIdComparator{}));
      for (const auto& t : range) {
        Acts::Vector3D particlePos(t.tx, t.ty, t.tz);
        Acts::Vector3D particleMom(t.tpx, t.tpy, t.tpz);
        // The following values are global to the particle and are not
        // duplicated in the per-hit file. They can be retrieved from
        // the particles file.
        double   charge = 0;
        double   mass   = 0;
        pdg_type pdgId  = 0;
        // TODO ownership
        particles.emplace_back(new FW::Data::SimParticle(
            particlePos, particleMom, mass, charge, pdgId, t.particle_id));
      }
    }

    // find matching pixel cell information
    std::vector<Acts::DigitizationCell> digitizationCells;
    {
      auto range = makeRange(std::equal_range(
          cells.begin(), cells.end(), hit.hit_id, HitIdComparator{}));
      for (const auto& c : range) {
        digitizationCells.emplace_back(c.ch0, c.ch1, c.value);
      }
    }

    // transform into local coordinates on the surface
    Acts::Vector3D pos(hit.x, hit.y, hit.z);
    Acts::Vector3D mom(1, 1, 1);  // fake momentum
    Acts::Vector2D local(0, 0);
    surface.globalToLocal(ctx.geoContext, pos, mom, local);

    // TODO what to use as cluster uncertainty?
    Acts::ActsSymMatrixD<2> cov = Acts::ActsSymMatrixD<2>::Identity();

    // create the planar cluster
    Acts::PlanarModuleCluster cluster(
        surface.getSharedPtr(),
        Identifier(Identifier::identifier_type(geoID.value()), particles),
        std::move(cov),
        local[0],
        local[1],
        std::move(digitizationCells));
    FW::Data::insert(clusters,
                     static_cast<geo_id_value>(hit.volume_id),
                     static_cast<geo_id_value>(hit.layer_id),
                     static_cast<geo_id_value>(hit.module_id),
                     std::move(cluster));
  }

  // write the data to the EventStore
  ctx.eventStore.add(m_cfg.outputClusters, std::move(clusters));
  ctx.eventStore.add(m_cfg.outputHitParticleMap, std::move(hitParticleMap));

  return FW::ProcessCode::SUCCESS;
}
