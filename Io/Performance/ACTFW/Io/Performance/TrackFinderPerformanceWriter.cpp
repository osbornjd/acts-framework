// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Io/Performance/TrackFinderPerformanceWriter.hpp"

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <vector>

#include <TFile.h>
#include <TTree.h>

#include "ACTFW/EventData/IndexContainers.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTFW/Utilities/Range.hpp"
#include "ACTFW/Validation/ProtoTrackClassification.hpp"
#include "Acts/Utilities/Helpers.hpp"
#include "Acts/Utilities/Units.hpp"
#include "ActsFatras/EventData/Barcode.hpp"

namespace {
using SimParticleContainer = FW::SimParticleContainer;
using HitParticlesMap      = FW::IndexMultimap<ActsFatras::Barcode>;
using ProtoTrackContainer  = FW::ProtoTrackContainer;
}  // namespace

struct FW::TrackFinderPerformanceWriter::Impl
{
  Config cfg;
  TFile* file = nullptr;

  // per-track tree
  TTree*     trkTree = nullptr;
  std::mutex trkMutex;
  // track identification
  ULong64_t trkEventId;
  ULong64_t trkTrackId;
  // track content
  // number of hits on track
  UShort_t trkNumHits;
  // number of particles contained in the track
  UShort_t trkNumParticles;
  // track particle content; for each contributing particle, largest first
  std::vector<ULong64_t> trkParticleId;
  // total number of hits generated by this particle
  std::vector<UShort_t> trkParticleNumHitsTotal;
  // number of hits within this track
  std::vector<UShort_t> trkParticleNumHitsOnTrack;

  // per-particle tree
  TTree*     prtTree = nullptr;
  std::mutex prtMutex;
  // particle identification
  ULong64_t prtEventId;
  ULong64_t prtParticleId;
  Int_t     prtParticleType;
  // particle kinematics
  // vertex position in mm
  float prtVx, prtVy, prtVz;
  // vertex time in ns
  float prtVt;
  // particle momentum at production in GeV
  float prtPx, prtPy, prtPz;
  // particle mass in GeV
  float prtM;
  // particle charge in e
  float prtQ;
  // particle reconstruction
  UShort_t prtNumHits;    // number of hits for this particle
  UShort_t prtNumTracks;  // number of tracks this particle was reconstructed in
  UShort_t prtNumTracksMajority;  // number of tracks reconstructed as majority
  // extra logger reference for the logging macros
  const Acts::Logger& _logger;

  Impl(Config&& c, const Acts::Logger& l) : cfg(std::move(c)), _logger(l)
  {
    if (cfg.inputParticles.empty()) {
      throw std::invalid_argument("Missing particles input collection");
    }
    if (cfg.inputHitParticlesMap.empty()) {
      throw std::invalid_argument("Missing hit-particles map input collection");
    }
    if (cfg.inputProtoTracks.empty()) {
      throw std::invalid_argument("Missing proto tracks input collection");
    }
    if (cfg.outputFilename.empty()) {
      throw std::invalid_argument("Missing output filename");
    }

    // the output file can not be given externally since TFile accesses to the
    // same file from multiple threads are unsafe.
    // must always be opened internally
    auto path = joinPaths(cfg.outputDir, cfg.outputFilename);
    file      = TFile::Open(path.c_str(), "RECREATE");
    if (not file) {
      throw std::invalid_argument("Could not open '" + path + "'");
    }

    // construct trees
    trkTree = new TTree("track_finder_tracks", "");
    trkTree->SetDirectory(file);
    trkTree->Branch("event_id", &trkEventId);
    trkTree->Branch("track_id", &trkTrackId);
    trkTree->Branch("size", &trkNumHits);
    trkTree->Branch("nparticles", &trkNumParticles);
    trkTree->Branch("particle_id", &trkParticleId);
    trkTree->Branch("particle_nhits_total", &trkParticleNumHitsTotal);
    trkTree->Branch("particle_nhits_on_track", &trkParticleNumHitsOnTrack);
    prtTree = new TTree("track_finder_particles", "");
    prtTree->SetDirectory(file);
    prtTree->Branch("event_id", &prtEventId);
    prtTree->Branch("particle_id", &prtParticleId);
    prtTree->Branch("particle_type", &prtParticleType);
    prtTree->Branch("vx", &prtVx);
    prtTree->Branch("vy", &prtVy);
    prtTree->Branch("vz", &prtVz);
    prtTree->Branch("vt", &prtVt);
    prtTree->Branch("px", &prtPx);
    prtTree->Branch("py", &prtPy);
    prtTree->Branch("pz", &prtPz);
    prtTree->Branch("m", &prtM);
    prtTree->Branch("q", &prtQ);
    prtTree->Branch("nhits", &prtNumHits);
    prtTree->Branch("ntracks", &prtNumTracks);
    prtTree->Branch("ntracks_majority", &prtNumTracksMajority);
  }

  const Acts::Logger&
  logger() const
  {
    return _logger;
  }

  void
  write(uint64_t                    eventId,
        const SimParticleContainer& particles,
        const HitParticlesMap&      hitParticlesMap,
        const ProtoTrackContainer&  tracks)
  {
    // compute the inverse mapping on-the-fly
    const auto& particleHitsMap = invertIndexMultimap(hitParticlesMap);
    // How often a particle was reconstructed.
    std::unordered_map<ActsFatras::Barcode, std::size_t> reconCount;
    reconCount.reserve(particles.size());
    // How often a particle was reconstructed as the majority particle.
    std::unordered_map<ActsFatras::Barcode, std::size_t> majorityCount;
    majorityCount.reserve(particles.size());
    // For each particle within a track, how many hits did it contribute
    std::vector<ParticleHitCount> particleHitCounts;

    // write per-track performance measures
    {
      std::lock_guard<std::mutex> guardTrk(trkMutex);
      for (size_t itrack = 0; itrack < tracks.size(); ++itrack) {
        const auto& track = tracks[itrack];

        identifyContributingParticles(
            hitParticlesMap, track, particleHitCounts);
        // extract per-particle reconstruction counts
        // empty track hits counts could originate from a  buggy track finder
        // that results in empty tracks or from purely noise track where no hits
        // is from a particle.
        if (not particleHitCounts.empty()) {
          auto it = majorityCount
                        .try_emplace(particleHitCounts.front().particleId, 0u)
                        .first;
          it->second += 1;
        }
        for (const auto& hc : particleHitCounts) {
          auto it = reconCount.try_emplace(hc.particleId, 0u).first;
          it->second += 1;
        }

        trkEventId      = eventId;
        trkTrackId      = itrack;
        trkNumHits      = track.size();
        trkNumParticles = particleHitCounts.size();
        trkParticleId.clear();
        trkParticleNumHitsTotal.clear();
        trkParticleNumHitsOnTrack.clear();
        for (const auto& phc : particleHitCounts) {
          trkParticleId.push_back(phc.particleId.value());
          // count total number of hits for this particle
          auto trueParticleHits
              = makeRange(particleHitsMap.equal_range(phc.particleId.value()));
          trkParticleNumHitsTotal.push_back(trueParticleHits.size());
          trkParticleNumHitsOnTrack.push_back(phc.hitCount);
        }

        trkTree->Fill();
      }
    }

    // write per-particle performance measures
    {
      std::lock_guard<std::mutex> guardPrt(trkMutex);
      for (const auto& particle : particles) {
        // find all hits for this particle
        auto hits
            = makeRange(particleHitsMap.equal_range(particle.particleId()));

        // identification
        prtEventId      = eventId;
        prtParticleId   = particle.particleId().value();
        prtParticleType = particle.pdg();
        // kinematics
        prtVx        = particle.position().x() / Acts::UnitConstants::mm;
        prtVy        = particle.position().y() / Acts::UnitConstants::mm;
        prtVz        = particle.position().z() / Acts::UnitConstants::mm;
        prtVt        = particle.time() / Acts::UnitConstants::ns;
        const auto p = particle.absMomentum() / Acts::UnitConstants::GeV;
        prtPx        = p * particle.unitDirection().x();
        prtPy        = p * particle.unitDirection().y();
        prtPz        = p * particle.unitDirection().z();
        prtM         = particle.mass() / Acts::UnitConstants::GeV;
        prtQ         = particle.charge() / Acts::UnitConstants::e;
        // reconstruction
        prtNumHits           = hits.size();
        auto nt              = reconCount.find(particle.particleId());
        prtNumTracks         = (nt != reconCount.end()) ? nt->second : 0u;
        auto nm              = majorityCount.find(particle.particleId());
        prtNumTracksMajority = (nm != majorityCount.end()) ? nm->second : 0u;

        prtTree->Fill();
      }
    }
  }
  /// Write everything to disk and close the file.
  void
  close()
  {
    if (not file) {
      ACTS_ERROR("Output file is not available");
      return;
    }
    file->Write();
    file->Close();
  }
};

FW::TrackFinderPerformanceWriter::TrackFinderPerformanceWriter(
    FW::TrackFinderPerformanceWriter::Config cfg,
    Acts::Logging::Level                     lvl)
  : WriterT(cfg.inputProtoTracks, "TrackFinderPerformanceWriter", lvl)
  , m_impl(std::make_unique<Impl>(std::move(cfg), logger()))
{
}

FW::TrackFinderPerformanceWriter::~TrackFinderPerformanceWriter()
{
  // explicit destructor needed for pimpl idiom to work
}

FW::ProcessCode
FW::TrackFinderPerformanceWriter::writeT(const FW::AlgorithmContext&    ctx,
                                         const FW::ProtoTrackContainer& tracks)
{
  const auto& particles
      = ctx.eventStore.get<SimParticleContainer>(m_impl->cfg.inputParticles);
  const auto& hitParticlesMap
      = ctx.eventStore.get<HitParticlesMap>(m_impl->cfg.inputHitParticlesMap);
  m_impl->write(ctx.eventNumber, particles, hitParticlesMap, tracks);
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::TrackFinderPerformanceWriter::endRun()
{
  m_impl->close();
  return ProcessCode::SUCCESS;
}
