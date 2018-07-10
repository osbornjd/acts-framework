// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @date 2016-05-23 Initial version
/// @date 2017-08-07 Rewrite with new interfaces

#pragma once

#include <mutex>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "Fatras/Kernel/Particle.hpp"

class TFile;
class TTree;

namespace FW {
namespace Root {

  /// Write out a particles associated to process vertices into a TTree
  ///
  /// Each entry in the TTree corresponds to the particles in one event.
  class RootParticleWriter final : public WriterT<std::vector<Fatras::Vertex>>
  {
  public:
    using Base = WriterT<std::vector<Fatras::Vertex>>;
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
    writeT(const AlgorithmContext&            ctx,
           const std::vector<Fatras::Vertex>& vertices) final override;

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
