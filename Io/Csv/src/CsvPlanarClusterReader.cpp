// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Io/Csv/CsvPlanarClusterReader.hpp"

#include <Acts/Plugins/Digitization/PlanarModuleCluster.hpp>
#include <Acts/Plugins/Identification/IdentifiedDetectorElement.hpp>
#include <Acts/Utilities/Units.hpp>
#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTFW/Utilities/Range.hpp"
#include "TrackMlData.hpp"

FW::CsvPlanarClusterReader::CsvPlanarClusterReader(
    const FW::CsvPlanarClusterReader::Config& cfg,
    Acts::Logging::Level                      lvl)
  : m_cfg(cfg)
  // TODO check that all files (hits,cells,truth) exists
  , m_eventsRange(determineEventFilesRange(cfg.inputDir, "hits.csv"))
  , m_logger(Acts::getDefaultLogger("CsvPlanarClusterReader", lvl))
{
  if (not m_cfg.trackingGeometry) {
    throw std::invalid_argument("Missing tracking geometry");
  }
  if (m_cfg.outputClusters.empty()) {
    throw std::invalid_argument("Missing cluster output collection");
  }
  if (m_cfg.outputHitIds.empty()) {
    throw std::invalid_argument("Missing hit id output collection");
  }
  if (m_cfg.outputHitParticlesMap.empty()) {
    throw std::invalid_argument("Missing hit-particles map output collection");
  }
  if (m_cfg.outputSimulatedHits.empty()) {
    throw std::invalid_argument("Missing simulated hits output collection");
  }
  // fill the geo id to surface map once to speed up lookups later on
  m_cfg.trackingGeometry->visitSurfaces([this](const Acts::Surface* surface) {
    this->m_surfaces[surface->geoID()] = surface;
  });
}

std::string
FW::CsvPlanarClusterReader::CsvPlanarClusterReader::name() const
{
  return "CsvPlanarClusterReader";
}

std::pair<size_t, size_t>
FW::CsvPlanarClusterReader::availableEvents() const
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
extractGeometryId(const FW::SimHitData& data)
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
  operator()(const FW::SimHitData& left, const FW::SimHitData& right) const
  {
    auto leftId  = extractGeometryId(left).value();
    auto rightId = extractGeometryId(right).value();
    return leftId < rightId;
  }
};

template <typename Data>
inline std::vector<Data>
readEverything(const std::string&              inputDir,
               const std::string&              filename,
               const std::vector<std::string>& optional_columns,
               size_t                          event)
{
  std::string path = FW::perEventFilepath(inputDir, filename, event);
  dfe::NamedTupleCsvReader<Data> reader(path, optional_columns);

  std::vector<Data> everything;
  Data              one;
  while (reader.read(one)) { everything.push_back(one); }

  return everything;
}

std::vector<FW::TruthHitData>
readTruthHitsByHitId(const std::string& inputDir, size_t event)
{
  // tt is an optional element
  auto truths
      = readEverything<FW::TruthHitData>(inputDir, "truth.csv", {"tt"}, event);
  // sort for fast hit id look up
  std::sort(truths.begin(), truths.end(), CompareHitId{});
  return truths;
}

std::vector<FW::SimHitData>
readSimHitsByGeoId(const std::string& inputDir, size_t event)
{
  // t is an optional element
  auto hits
      = readEverything<FW::SimHitData>(inputDir, "hits.csv", {"t"}, event);
  // sort same way they will be sorted in the output container
  std::sort(hits.begin(), hits.end(), CompareGeometryId{});
  return hits;
}

std::vector<FW::CellData>
readCellsByHitId(const std::string& inputDir, size_t event)
{
  // timestamp is an optional element
  auto cells = readEverything<FW::CellData>(
      inputDir, "cells.csv", {"timestamp"}, event);
  // sort for fast hit id look up
  std::sort(cells.begin(), cells.end(), CompareHitId{});
  return cells;
}

}  // namespace

FW::ProcessCode
FW::CsvPlanarClusterReader::read(const FW::AlgorithmContext& ctx)
{
  // hit_id in the files is not required to be neither continuous nor
  // monotonic. internally, we want continous indices within [0,#hits)
  // to simplify data handling. to be able to perform this mapping we first
  // read all data into memory before converting to the internal event data
  // types.
  auto truths = readTruthHitsByHitId(m_cfg.inputDir, ctx.eventNumber);
  auto hits   = readSimHitsByGeoId(m_cfg.inputDir, ctx.eventNumber);
  auto cells  = readCellsByHitId(m_cfg.inputDir, ctx.eventNumber);

  // prepare containers for the hit data using the framework event data types
  GeometryIdMultimap<Acts::PlanarModuleCluster> clusters;
  std::vector<uint64_t>                         hitIds;
  IndexMultimap<Barcode>                        hitParticlesMap;
  SimHits                                       simHits;
  clusters.reserve(hits.size());
  hitIds.reserve(hits.size());
  hitParticlesMap.reserve(truths.size());
  simHits.reserve(truths.size());

  for (const SimHitData& hit : hits) {

    // identify hit surface
    Acts::GeometryID geoId = extractGeometryId(hit);
    auto             it    = m_surfaces.find(geoId);
    if (it == m_surfaces.end() or not it->second) {
      ACTS_FATAL("Could not retrieve the surface for hit " << hit);
      return ProcessCode::ABORT;
    }
    const Acts::Surface& surface = *(it->second);

    // find associated truth hits and their particle data.
    std::vector<const FW::Data::SimParticle*> particles;
    {
      auto range = makeRange(std::equal_range(
          truths.begin(), truths.end(), hit.hit_id, CompareHitId{}));
      for (const auto& truth : range) {

        FW::Data::SimHit simHit(surface);
        simHit.position  = Acts::Vector3D(truth.tx * Acts::UnitConstants::mm,
                                         truth.ty * Acts::UnitConstants::mm,
                                         truth.tz * Acts::UnitConstants::mm);
        simHit.time      = truth.tt * Acts::UnitConstants::ns;
        simHit.direction = Acts::Vector3D(truth.tpx * Acts::UnitConstants::GeV,
                                          truth.tpy * Acts::UnitConstants::GeV,
                                          truth.tpz * Acts::UnitConstants::GeV);
        // TODO extract hit value/charge from cells
        simHit.value = 0;
        // Mass, charge, and PDG identifier are global to the particle and are
        // not duplicated in the per-hit truth file. They could be retrieved
        // from the particles file, but are set to bogus values for now to
        // simplify the implementation.
        simHit.particle = FW::Data::SimParticle(simHit.position,
                                                simHit.direction,
                                                0,
                                                0,
                                                0,
                                                truth.particle_id,
                                                simHit.time);
        // hit should only store direction not full momentum
        simHit.direction.normalize();

        // the cluster stores pointers to the underlying particles. thus their
        // memory location must be stable. the preordering of hits by geometry
        // id should ensure that new sim hits are always added at the end and
        // previously created ones rest at their existing locations. sufficient
        // underlying memory should have been allocated once at the beginning
        // and no reallocation should occur that could modify the memory
        // addresses.
        // checks are added to be safe.
        auto capacity = simHits.capacity();
        auto inserted = simHits.emplace_hint(simHits.end(), std::move(simHit));
        if (std::next(inserted) != simHits.end()) {
          ACTS_FATAL("Truth hit sorting broke for input hit id " << hit.hit_id);
          return ProcessCode::ABORT;
        }
        if (capacity != simHits.capacity()) {
          ACTS_FATAL(
              "Forbidden truth hits reallocation encountered for input hit id "
              << hit.hit_id);
          return ProcessCode::ABORT;
        }
        particles.push_back(&(inserted->particle));
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

    // transform global hit coordinates into local coordinates on the surface
    Acts::Vector3D pos(hit.x * Acts::UnitConstants::mm,
                       hit.y * Acts::UnitConstants::mm,
                       hit.z * Acts::UnitConstants::mm);
    double         time = hit.t * Acts::UnitConstants::ns;
    Acts::Vector3D mom(1, 1, 1);  // fake momentum
    Acts::Vector2D local(0, 0);
    surface.globalToLocal(ctx.geoContext, pos, mom, local);
    // TODO what to use as cluster uncertainty?
    Acts::ActsSymMatrixD<3> cov = Acts::ActsSymMatrixD<3>::Identity();
    // create the planar cluster
    Acts::PlanarModuleCluster cluster(
        surface.getSharedPtr(),
        Identifier(Identifier::identifier_type(geoId.value()),
                   std::move(particles)),
        std::move(cov),
        local[0],
        local[1],
        time,
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
    auto hitIndex   = clusters.index_of(inserted);
    auto truthRange = makeRange(std::equal_range(
        truths.begin(), truths.end(), hit.hit_id, CompareHitId{}));
    for (const auto& truth : truthRange) {
      hitParticlesMap.emplace_hint(
          hitParticlesMap.end(), hitIndex, truth.particle_id);
    }

    // map internal hit/cluster index back to original, non-monotonic hit id
    hitIds.push_back(hit.hit_id);
  }

  // write the data to the EventStore
  ctx.eventStore.add(m_cfg.outputClusters, std::move(clusters));
  ctx.eventStore.add(m_cfg.outputHitIds, std::move(hitIds));
  ctx.eventStore.add(m_cfg.outputHitParticlesMap, std::move(hitParticlesMap));
  ctx.eventStore.add(m_cfg.outputSimulatedHits, std::move(simHits));

  return FW::ProcessCode::SUCCESS;
}
