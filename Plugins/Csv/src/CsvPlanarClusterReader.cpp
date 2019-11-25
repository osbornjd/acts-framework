// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"

#include <Acts/Plugins/Digitization/PlanarModuleCluster.hpp>
#include <Acts/Plugins/Identification/IdentifiedDetectorElement.hpp>
#include <Acts/Utilities/Units.hpp>
#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTFW/Utilities/Range.hpp"
#include "TrackMlData.hpp"

FW::Csv::CsvPlanarClusterReader::CsvPlanarClusterReader(
    const FW::Csv::CsvPlanarClusterReader::Config& cfg,
    Acts::Logging::Level                           level)
  : m_cfg(cfg)
  // TODO check that all files (hits,cells,truth) exists
  , m_eventsRange(determineEventFilesRange(cfg.inputDir, "hits.csv"))
  , m_logger(Acts::getDefaultLogger("CsvPlanarClusterReader", level))
{
  if (not m_cfg.trackingGeometry) {
    throw std::invalid_argument("Missing tracking geometry");
  }
  if (m_cfg.outputClusters.empty()) {
    throw std::invalid_argument("Missing cluster output collection");
  }
  if (m_cfg.outputHitParticlesMap.empty()) {
    throw std::invalid_argument("Missing hit-particles map output collection");
  }
  if (m_cfg.outputHitIds.empty()) {
    throw std::invalid_argument("Missing hit id output collection");
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

std::pair<size_t, size_t>
FW::Csv::CsvPlanarClusterReader::availableEvents() const
{
  return m_eventsRange;
}

namespace {
struct CompareHitId
{
  // support transparent comparision between identifiers and full objects
  using is_transparent = void;
  template <typename T>
  constexpr bool
  operator()(const T& left, const T& right) const
  {
    return left.hit_id < right.hit_id;
  }
  template <typename T>
  constexpr bool
  operator()(uint64_t left_id, const T& right) const
  {
    return left_id < right.hit_id;
  }
  template <typename T>
  constexpr bool
  operator()(const T& left, uint64_t right_id) const
  {
    return left.hit_id < right_id;
  }
};

/// Convert separate volume/layer/module id into a single geometry identifier.
inline Acts::GeometryID
extractGeometryId(const FW::HitData& data)
{
  Acts::GeometryID geoId;
  geoId.setVolume(data.volume_id);
  geoId.setLayer(data.layer_id);
  geoId.setSensitive(data.module_id);
  return geoId;
}

struct CompareGeometryId
{
  bool
  operator()(const FW::HitData& left, const FW::HitData& right) const
  {
    auto leftId  = extractGeometryId(left).value();
    auto rightId = extractGeometryId(right).value();
    return leftId < rightId;
  }
};

template <typename Data>
inline std::vector<Data>
readEverything(const std::string& inputDir,
               const std::string& filename,
               size_t             event)
{
  std::string path = FW::perEventFilepath(inputDir, filename, event);
  dfe::CsvNamedTupleReader<Data> reader(path);

  std::vector<Data> everything;
  Data              one;
  while (reader.read(one)) { everything.push_back(one); }

  return everything;
}

std::vector<FW::TruthData>
readTruthByHitId(const std::string& inputDir, size_t event)
{
  auto truths = readEverything<FW::TruthData>(inputDir, "truth.csv", event);
  // sort for fast hit id look up
  std::sort(truths.begin(), truths.end(), CompareHitId{});
  return truths;
}

std::vector<FW::CellData>
readCellsByHitId(const std::string& inputDir, size_t event)
{
  auto cells = readEverything<FW::CellData>(inputDir, "cells.csv", event);
  // sort for fast hit id look up
  std::sort(cells.begin(), cells.end(), CompareHitId{});
  return cells;
}

std::vector<FW::HitData>
readHitsByGeoId(const std::string& inputDir, size_t event)
{
  auto hits = readEverything<FW::HitData>(inputDir, "hits.csv", event);
  // sort same way they will be sorted in the output container
  std::sort(hits.begin(), hits.end(), CompareGeometryId{});
  return hits;
}

}  // namespace

FW::ProcessCode
FW::Csv::CsvPlanarClusterReader::read(const FW::AlgorithmContext& ctx)
{
  // hit_id in the files is not required to be neither continuous nor
  // monotonic. internally, we want continous indices within [0,#hits)
  // to simplify data handling. to be able to perform this mapping we first
  // read all data into memory before converting to the internal representation.
  auto truths = readTruthByHitId(m_cfg.inputDir, ctx.eventNumber);
  auto cells  = readCellsByHitId(m_cfg.inputDir, ctx.eventNumber);
  auto hits   = readHitsByGeoId(m_cfg.inputDir, ctx.eventNumber);

  // convert into internal representations
  GeometryIdMultimap<Acts::PlanarModuleCluster> clusters;
  IndexMultimap<barcode_type>                   hitParticlesMap;
  std::vector<uint64_t>                         hitIds;
  clusters.reserve(hits.size());
  hitParticlesMap.reserve(hits.size());
  hitIds.reserve(hits.size());
  for (const HitData& hit : hits) {

    // identify surface
    Acts::GeometryID geoId = extractGeometryId(hit);
    auto             it    = m_surfaces.find(geoId);
    if (it == m_surfaces.end() or not it->second) {
      ACTS_FATAL("Could not retrieve the surface for hit " << hit);
      return ProcessCode::ABORT;
    }
    const Acts::Surface& surface = *(it->second);

    // find matching truth particle information
    // TODO who owns these particles?
    std::vector<const FW::Data::SimParticle*> particles;
    {
      auto range = makeRange(std::equal_range(
          truths.begin(), truths.end(), hit.hit_id, CompareHitId{}));
      for (const auto& t : range) {
        Acts::Vector3D particlePos(t.tx * Acts::UnitConstants::mm,
                                   t.ty * Acts::UnitConstants::mm,
                                   t.tz * Acts::UnitConstants::mm);
        double         particleTime = t.tt * Acts::UnitConstants::ns;
        Acts::Vector3D particleMom(t.tpx * Acts::UnitConstants::GeV,
                                   t.tpy * Acts::UnitConstants::GeV,
                                   t.tpz * Acts::UnitConstants::GeV);
        // The following values are global to the particle and are not
        // duplicated in the per-hit file. They can be retrieved from
        // the particles file.
        double   charge = 0;
        double   mass   = 0;
        pdg_type pdgId  = 0;
        // TODO ownership
        particles.emplace_back(new FW::Data::SimParticle(particlePos,
                                                         particleMom,
                                                         mass,
                                                         charge,
                                                         pdgId,
                                                         t.particle_id,
                                                         particleTime));
      }
    }

    // find matching pixel cell information
    std::vector<Acts::DigitizationCell> digitizationCells;
    {
      auto range = makeRange(std::equal_range(
          cells.begin(), cells.end(), hit.hit_id, CompareHitId{}));
      for (const auto& c : range) {
        digitizationCells.emplace_back(c.ch0, c.ch1, c.value);
      }
    }

    // transform into local coordinates on the surface
    Acts::Vector3D pos(hit.x * Acts::UnitConstants::mm,
                       hit.y * Acts::UnitConstants::mm,
                       hit.z * Acts::UnitConstants::mm);
    // TODO use hit time once clusters store it
    Acts::Vector3D mom(1, 1, 1);  // fake momentum
    Acts::Vector2D local(0, 0);
    surface.globalToLocal(ctx.geoContext, pos, mom, local);
    // TODO what to use as cluster uncertainty?
    Acts::ActsSymMatrixD<3> cov = Acts::ActsSymMatrixD<3>::Identity();
    // create the planar cluster
    Acts::PlanarModuleCluster cluster(
        surface.getSharedPtr(),
        Identifier(Identifier::identifier_type(geoId.value()), particles),
        std::move(cov),
        local[0],
        local[1],
        hit.t,
        std::move(digitizationCells));

    // due to the previous sorting of the raw hit data by geometry id, new
    // clusters should always end up at the end of the container. previous
    // elements were not touched; cluster indices remain stable and can
    // be used to identify the hit.
    auto inserted
        = clusters.emplace_hint(clusters.end(), geoId, std::move(cluster));
    if (std::next(inserted) != clusters.end()) {
      ACTS_FATAL("Something went horribly wrong with the hit sorting");
      return ProcessCode::ABORT;
    }
    auto hitIndex            = clusters.index_of(inserted);
    auto generatingParticles = makeRange(std::equal_range(
        truths.begin(), truths.end(), hit.hit_id, CompareHitId{}));
    for (const auto& particle : generatingParticles) {
      hitParticlesMap.emplace_hint(
          hitParticlesMap.end(), hitIndex, particle.particle_id);
    }

    // map internal hit/cluster index back to original, non-monotonic hit id
    hitIds.push_back(hit.hit_id);
  }

  // write the data to the EventStore
  ctx.eventStore.add(m_cfg.outputClusters, std::move(clusters));
  ctx.eventStore.add(m_cfg.outputHitParticlesMap, std::move(hitParticlesMap));
  ctx.eventStore.add(m_cfg.outputHitIds, std::move(hitIds));

  return FW::ProcessCode::SUCCESS;
}
