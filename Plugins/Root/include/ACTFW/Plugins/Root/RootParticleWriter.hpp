//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
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

  /// @class RootParticleWriter
  ///
  /// A root based implementation to write out particleproperties vector
  ///
  class RootParticleWriter final
    : public WriterT<std::vector<Acts::ParticleProperties>>
  {
  public:
    using Base = WriterT<std::vector<Acts::ParticleProperties>>;
    struct Config
    {
      std::string collection;              ///< particle collection to write
      std::string fileName;                ///< name of the output file
      std::string fileMode = "RECREATE";   ///< file access mode
      std::string treeName = "particles";  ///< name of the output tree
      std::shared_ptr<FW::BarcodeSvc>
          barcodeSvc;  ///< the barcode service to decode
    };

    RootParticleWriter(const Config&        cfg,
                       Acts::Logging::Level level = Acts::Logging::INFO);
    virtual ~RootParticleWriter() = default;

    ProcessCode
    initialize() final;
    ProcessCode
    finalize() final;

  protected:
    ProcessCode
    writeT(const AlgorithmContext&                      ctx,
           const std::vector<Acts::ParticleProperties>& particles) final;

  private:
    Config     m_cfg;         ///< the config class
    std::mutex m_writeMutex;  ///< mutex used to protect multi-threaded writes
    TFile*     m_outputFile;  ///< the output file name
    TTree*     m_outputTree;  ///< the output tree name
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
