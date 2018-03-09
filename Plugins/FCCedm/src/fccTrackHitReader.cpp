///////////////////////////////////////////////////////////////////
// fccTrackHitReader.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/Plugins/FCCedm/fccTrackHitReader.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "TChain.h"
#include "TFile.h"

FW::FCCedm::fccTrackHitReader::fccTrackHitReader(
    const Config&                       cfg,
    std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg)
  , m_logger(std::move(logger))
  , m_treeReader(nullptr)
  , m_measurements()
  , m_particleMap()
{
  if (m_cfg.fileList.empty()) {
    throw std::invalid_argument(
        "Missing fileName(s) of the file(s) to be read in.");
  }
  if (m_cfg.treeName.empty()) {
    throw std::invalid_argument(
        "Missing treeName of the file(s) to be read in.");
  }
  // get the input files
  auto inputChain = new TChain(m_cfg.treeName.c_str());
  // loop over the input files
  for (auto inputFile : m_cfg.fileList) {
    // add file to the input chain
    inputChain->Add(inputFile.c_str());
  }
  // read in files
  m_treeReader = new TTreeReader(inputChain);
  // set the number of entries
  m_nEvents = m_treeReader->GetEntries(true);
  // read in fcc::positionedTrackHits
  m_positionedTrackHits
      = TTreeReaderValue<std::vector<fcc::PositionedTrackHitData>>(
          *m_treeReader, m_cfg.branchName.c_str());
  // read in fcc::MCParticles
  m_particles = TTreeReaderValue<std::vector<fcc::MCParticleData>>(
      *m_treeReader, "simParticles");

  m_detElements
      = m_cfg.trackingGeometry->highestTrackingVolume()->detectorElements();
}

std::string
FW::FCCedm::fccTrackHitReader::name() const
{
  return "fccTrackHitReader";
}

FW::ProcessCode
FW::FCCedm::fccTrackHitReader::skip(size_t nEvents)
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::FCCedm::fccTrackHitReader::read(FW::AlgorithmContext ctx)
{
  // exclusive access to the tree
  std::lock_guard<std::mutex> lock(m_read_mutex);
  // If there is no entry left stop reading
  if (!m_treeReader->Next()) return ProcessCode::END;

  ACTS_DEBUG("Reading in " << m_positionedTrackHits->size()
                           << " fcc positioned track hits");
  m_measurements.clear();
  // reserve space
  m_measurements.reserve(m_positionedTrackHits->size());
  // translate FCC edm PositionedTrackHits to FW::fccMeasurement for
  // further processing
  for (auto& ptHit : *m_positionedTrackHits)
    m_measurements.push_back(measurement(ptHit));

  m_particleMap.clear();
  // translate FCC edm MCParticle to FW::fccTruthParticle for
  // further processing
  for (auto& truthP : *m_particles)
    m_particleMap.emplace(std::make_pair(truthP.core.bits, particle(truthP)));

  // m_particleMap[0](particle(truthP));

  // write to the EventStore
  if (ctx.eventStore.add(m_cfg.collection, std::move(m_measurements))
      == FW::ProcessCode::ABORT) {
    ACTS_INFO("Could not add collection " << m_cfg.collection
                                          << " to event store. Abort.");
    return FW::ProcessCode::ABORT;
  }
  if (ctx.eventStore.add(m_cfg.particleMap, std::move(m_particleMap))
      == FW::ProcessCode::ABORT) {
    ACTS_INFO("Could not add collection " << m_cfg.particleMap
                                          << " to event store. Abort.");
    return FW::ProcessCode::ABORT;
  }

  return ProcessCode::SUCCESS;
}

const FW::fccMeasurement
FW::FCCedm::fccTrackHitReader::measurement(
    const fcc::PositionedTrackHitData& fccTrackHit) const
{
  // access cellID of hit
  auto cellID = fccTrackHit.core.cellId;
  // convert cellID to the ID of the surface
  Identifier identifier(cellID & m_cfg.mask);
  // the global position of the hit
  Acts::Vector3D globPos(fccTrackHit.position.x * Acts::units::_mm,
                         fccTrackHit.position.y * Acts::units::_mm,
                         fccTrackHit.position.z * Acts::units::_mm);
  // get the corresponding cell
  auto detElement = m_detElements.find(identifier);
  if (detElement != m_detElements.end()) {
    // translate global to local position
    Acts::Vector2D locPos(0., 0.);
    (*detElement)
        .second->surface()
        .globalToLocal(globPos, Acts::Vector3D(0., 0., 0.), locPos);
    // the covariance
    Acts::ActsSymMatrixD<2> cov;

    return (fccMeasurement((*detElement).second->surface(),
                           identifier,
                           std::move(cov),
                           locPos.x(),
                           locPos.y(),
                           fccTrackHit.core.bits));
  }
  std::string error = "No detector element exists for requested identifier: "
      + std::to_string(identifier) + ", at the global position: ("
      + std::to_string(globPos.x()) + "," + std::to_string(globPos.y()) + ","
      + std::to_string(globPos.z()) + ")";
  throw std::runtime_error(error);
}

const FW::fccTruthParticle
FW::FCCedm::fccTrackHitReader::particle(
    const fcc::MCParticleData& fccParticle) const
{
  Acts::Vector3D vertex(fccParticle.core.vertex.x,
                        fccParticle.core.vertex.y,
                        fccParticle.core.vertex.z);
  unsigned       status = fccParticle.core.status;
  Acts::Vector3D momentum(
      fccParticle.core.p4.px, fccParticle.core.p4.py, fccParticle.core.p4.pz);
  double       mass    = 0.;
  double       charge  = fccParticle.core.charge;
  pdg_type     pID     = fccParticle.core.pdgId;
  barcode_type barcode = fccParticle.core.bits;
  return (FW::fccTruthParticle(
      vertex, status, momentum, mass, charge, pID, barcode));
}
