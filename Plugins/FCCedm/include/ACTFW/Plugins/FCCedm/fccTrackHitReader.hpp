#ifndef ACTFW_PLUGINS_FCCEDM_TRACKHITREADER_H
#define ACTFW_PLUGINS_FCCEDM_TRACKHITREADER_H

#include <mutex>
#include "ACTFW/Framework/IReader.hpp"
#include "ACTFW/Utilities/HitData.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "TTree.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "datamodel/PositionedTrackHitData.h"

namespace Acts {
class DetectorElementBase;
class TrackingGeometry;
}

namespace FW {
namespace FCCedm {

  /// @class fccTrackHitReader
  ///
  /// @brief Reads in collection of fcc::PositionedTrackHit
  ///
  /// The fccTrackHitReader reads in a collection of
  /// fcc::PositionedTrackHit per event from given root file(s). It translates
  /// each fcc::PositionedTrackHit into a local Acts::Measurement (on a
  /// Acts::Surface) and writes the measurements to the event store.

  class fccTrackHitReader final : public FW::IReader
  {
  public:
    class Config
    {
    public:
      /// The names of the input files
      std::vector<std::string> fileList;
      /// The name of the input tree
      std::string treeName = "TTree";
      /// The name of the input branch
      std::string branchName = "TBranch";
      /// The positionedTrackHit collection to be stored in the event store
      std::string collection = "measurements";
      /// The tracking geometry (needed to assign the surfaces to the hits and
      /// calculate local position)
      std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry = nullptr;
      /// The mask to decode cellID to volumeID
      unsigned long long mask = 0xffffffff;
    };

    /// Constructor
    fccTrackHitReader(const Config&                       cfg,
                      std::unique_ptr<const Acts::Logger> logger
                      = Acts::getDefaultLogger("fccTrackHitReader",
                                               Acts::Logging::INFO));

    /// Desctructor
    ~fccTrackHitReader() = default;

    /// Framework name() method
    std::string
    name() const final override;

    /// Skip a few events in the IO stream
    /// @param [in] nEvents is the number of skipped events
    FW::ProcessCode
    skip(size_t nEvents) final override;

    /// Read out data from the input stream
    /// @param [in] ctx is the algorithm context that guarantees event
    ///        consistency
    FW::ProcessCode
    read(FW::AlgorithmContext ctx) final override;

    /// The number of events to be read in
    /// @return number of events
    size_t
    numEvents() const final override;

  private:
    /// The configuration object
    Config m_cfg;
    /// The logging instance
    std::unique_ptr<const Acts::Logger> m_logger;
    // The root tree reader
    TTreeReader* m_treeReader;
    /// Mutex used to protect multi-threaded reads
    std::mutex m_read_mutex;
    // A map of all contained detector elements and their corresponding
    /// identifier
    std::map<Identifier, const Acts::DetectorElementBase*> m_detElements;
    /// The FCC TrackHits to be read in
    // fcc::PositionedTrackHitCollection m_positionedTrackHits;
    TTreeReaderValue<std::vector<fcc::PositionedTrackHitData>>
        m_positionedTrackHits;
    /// The Acts measurments which are written to the store
    std::vector<FW::Measurement2D> m_measurements;
    /// The number of events, determined by the entries in the file
    size_t m_nEvents;

    /// @todo remove later, just for debugging
    mutable std::vector<Identifier> m_notFound;
    mutable std::vector<Identifier> m_found;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }

    /// Private class which translates the FCC positioned track hit into an Acts
    /// measurement
    /// @param[in] fccTrackHit The inout fcc positioned track hit
    /// @todo currently a pointer of a measurement is returned, this is because
    /// not all detector elements are found yet. Later just a Measurment should
    /// be returned.
    const FW::Measurement2D*
    measurement(const fcc::PositionedTrackHitData& fccTrackHit) const;
  };

  inline size_t
  fccTrackHitReader::numEvents() const
  {
    return m_nEvents;
  }
}  // namespace Root
}  // namespace FW

#endif  // ACTFW_PLUGINS_FCCEDM_TRACKHITREADER_H
