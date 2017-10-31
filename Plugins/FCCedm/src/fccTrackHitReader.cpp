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
{
  auto inputChain = new TChain(m_cfg.treeName.c_str());
  // loop over the input files
  for (auto inputFile : m_cfg.fileList) {
    // add file to the input chain
    inputChain->Add(inputFile.c_str());
  }

  m_treeReader = new TTreeReader(inputChain);

  m_positionedTrackHits
      = TTreeReaderValue<std::vector<fcc::PositionedTrackHitData>>(
          *m_treeReader, m_cfg.branchName.c_str());

  /// @todo remove all of this later
  m_detElements
      = m_cfg.trackingGeometry->highestTrackingVolume()->detectorElements();
  std::cout << "detelements size: " << m_detElements.size() << std::endl;
  m_notFound.reserve(44900);
  m_found.reserve(44900);
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
  // If there is no entry left end the reading
  if (!m_treeReader->Next()) return ProcessCode::END;

  ACTS_DEBUG("Reading in " << m_positionedTrackHits->size()
                           << " fcc positioned track hits");

  // reserve space
  m_measurements.reserve(m_positionedTrackHits->size());
  // translate FCC edm PositionedTrackHits to Acts measurenents for
  // further processing
  for (auto& ptHit : *m_positionedTrackHits) {
    auto m = measurement(ptHit);
    if (m) m_measurements.push_back(*m);
  }
  // write to the EventStore
  if (ctx.eventStore.add(m_cfg.collection, std::move(m_measurements))
      == FW::ProcessCode::ABORT) {
    ACTS_INFO("Could not add collection " << m_cfg.collection
                                          << " to event store. Abort.");
    return FW::ProcessCode::ABORT;
  }

  /// @todo remove
  std::vector<Identifier> keys;
  keys.reserve(m_positionedTrackHits->size());
  std::stable_sort(m_notFound.begin(), m_notFound.end());
  std::unique_copy(begin(m_notFound), end(m_notFound), back_inserter(keys));
  m_notFound = keys;
  std::cout << "#Not Found Elements: " << keys.size() << std::endl;

  std::vector<Identifier> keysF;
  keysF.reserve(m_positionedTrackHits->size());
  std::stable_sort(m_found.begin(), m_found.end());
  std::unique_copy(begin(m_found), end(m_found), back_inserter(keysF));
  m_found = keysF;
  std::cout << "#Found Elements: " << keysF.size() << std::endl;

  return ProcessCode::SUCCESS;
}

/// @todo currently pointer, remove later, when surfaces are found
const FW::Measurement2D*
FW::FCCedm::fccTrackHitReader::measurement(
    const fcc::PositionedTrackHitData& fccTrackHit) const
{
  // access cellID of hit
  auto cellID = fccTrackHit.core.cellId;
  // convert cellID to geometric identifier
  Identifier identifier(cellID & m_cfg.mask);
  // get the corresponding cell
  auto detElement = m_detElements.find(identifier);
  if (detElement != m_detElements.end()) {
    // translate global to local position
    Acts::Vector2D locPos(0., 0.);
    (*detElement)
        .second->surface()
        .globalToLocal(
            Acts::Vector3D(fccTrackHit.position.x * Acts::units::_mm,
                           fccTrackHit.position.y * Acts::units::_mm,
                           fccTrackHit.position.z * Acts::units::_mm),
            Acts::Vector3D(0., 0., 0.),
            locPos);
    // the covariance
    Acts::ActsSymMatrixD<2> cov;

    /// @todo remove later, just for debugging
    m_found.push_back(identifier);

    return (new Measurement2D((*detElement).second->surface(),
                              identifier,
                              std::move(cov),
                              locPos.x(),
                              locPos.y()));
  }
  /// @todo remove later, just for debugging
  m_notFound.push_back(identifier);
  // @todo uncomment
  // throw std::runtime_error(
  //"No detector element exists for requested identifier: " << identifier);
  return nullptr;
}
