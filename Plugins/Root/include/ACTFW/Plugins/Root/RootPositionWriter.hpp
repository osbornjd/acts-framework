#ifndef ACTFW_PLUGINS_ROOTPOSITIONWRITER_H
#define ACTFW_PLUGINS_ROOTPOSITIONWRITER_H

#include <mutex>
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTFW/Utilities/HitData.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;
class TTree;

namespace FW {

namespace Root {

  /// @class RootPositionWriter
  ///
  class RootPositionWriter : public FW::WriterT<std::vector<Acts::Vector3D>>
  {
  public:
    using Base = FW::WriterT<std::vector<Acts::Vector3D>>;

    // @struct Config
    //
    // The nested config class
    struct Config
    {
    public:
      /// The input parameters of the distance analysis for component
      std::string positions = "positions";
      /// The output file path
      std::string filePath = "positions.root";
      /// The file access mode
      std::string fileMode = "RECREATE";
      /// The name of the output tree
      std::string treeName = "positions";
    };

    /// Constructor
    /// @param cfg is the configuration class
    RootPositionWriter(const Config&        cfg,
                       Acts::Logging::Level level = Acts::Logging::INFO);

    /// Virtual destructor
    ~RootPositionWriter() override;

    /// End-of-run hook
    ProcessCode
    endRun() final override;

  protected:
    /// The protected writeT method, called by the WriterT base
    /// @param [in] ctx is the algorithm context for event consistency
    /// @param [in] positions the position to be written out
    ProcessCode
    writeT(const FW::AlgorithmContext&        ctx,
           const std::vector<Acts::Vector3D>& positions) final override;

    /// The config class
    Config m_cfg;
    /// Protect multi-threaded writes
    std::mutex m_writeMutex;
    /// The output file
    TFile* m_outputFile;
    /// The output tree
    TTree* m_outputTree;
    /// x position component
    float m_x;
    /// y position component
    float m_y;
    /// z position component
    float m_z;
    /// r of the position
    float m_r;
  };

}  // namespace Root
}  // namespace FW

#endif  // ACTFW_PLUGINS_ROOTPOSITIONWRITER_H
