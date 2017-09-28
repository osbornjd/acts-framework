/// @file
/// @date 2016-05-23 Initial version
/// @date 2017-08-07 Rewrite with new interfaces
/// @autor Andreas Salzburger
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_PLUGINS_ROOT_ROOTPARTICLEWRITER_H
#define ACTFW_PLUGINS_ROOT_ROOTPARTICLEWRITER_H

#include <mutex>
#include <ACTS/EventData/ParticleDefinitions.hpp>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WriterT.hpp"

class TFile;
class TTree;

namespace FW {
namespace Root {

  /// Write out a particles associated to process vertices into a TTree
  ///
  /// Each entry in the TTree corresponds to the particles in one event.
  class RootParticleWriter final
    : public WriterT<std::vector<Acts::ProcessVertex>>
  {
  public:
    using Base = WriterT<std::vector<Acts::ProcessVertex>>;
    struct Config
    {
      std::string collection;              ///< particle collection to write
      std::string filePath;                ///< path of the output file
      std::string fileMode = "RECREATE";   ///< file access mode
      std::string treeName = "particles";  ///< name of the output tree
      std::shared_ptr<FW::BarcodeSvc>
          barcodeSvc;  ///< the barcode service to decode (optional)
    };

    /// Constructor
    RootParticleWriter(const Config&        cfg,
                       Acts::Logging::Level level = Acts::Logging::INFO);

    /// Virtual destructor
    ~RootParticleWriter() override;

    /// End-of-run hook
    ProcessCode
    endRun() final override;

  protected:
    /// write method called by the base class
    /// @param [in] ctx is the algorithm context for consistency
    /// @param [in] vertices is the process vertex collection for the
    /// particles to be attached
    ProcessCode
    writeT(const AlgorithmContext&                      ctx,
           const std::vector<Acts::ProcessVertex>& vertices) final override;

  private:
    Config     m_cfg;         ///< the config class
    std::mutex m_writeMutex;  ///< mutex used to protect multi-threaded writes
    TFile*     m_outputFile;  ///< the output file 
    TTree*     m_outputTree;  ///< the output tree
    std::vector<float> m_vx;
    std::vector<float> m_vy;
    std::vector<float> m_vz;
    std::vector<float> m_px;
    std::vector<float> m_py;
    std::vector<float> m_pz;
    std::vector<float> m_pT;
    std::vector<float> m_eta;
    std::vector<float> m_phi;
    std::vector<float> m_mass;
    std::vector<int>   m_charge;
    std::vector<int>   m_pdgCode;
    std::vector<int>   m_barcode;
    std::vector<int>   m_vertex;
    std::vector<int>   m_primary;
    std::vector<int>   m_generation;
    std::vector<int>   m_secondary;
    std::vector<int>   m_process;
  };

}  // namespace Root
}  // namespace FW

#endif  // ACTFW_PLUGINS_ROOT_ROOTPARTICLEWRITER_H