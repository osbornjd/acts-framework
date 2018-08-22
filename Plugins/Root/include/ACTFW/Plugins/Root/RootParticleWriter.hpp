// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <mutex>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "ACTFW/EventData/SimParticle.hpp"

class TFile;
class TTree;

namespace FW {
namespace Root {

  /// Write out a particles associated to process vertices into a TTree
  ///
  /// Each entry in the TTree corresponds to the particles in one event.
  class RootParticleWriter final : public WriterT<std::vector<Data::Vertex>>
  {
  public:
    using Base = WriterT<std::vector<Data::Vertex>>;
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
    /// @brief Write method called by the base class
    /// @param [in] ctx is the algorithm context for consistency
    /// @param [in] vertices is the process vertex collection for the
    /// particles to be attached
    ProcessCode
    writeT(const AlgorithmContext&            ctx,
           const std::vector<Data::Vertex>& vertices) final override;

  private:
    Config     m_cfg;                 ///< The config class
    std::mutex m_writeMutex;          ///< Mutex used to protect multi-threaded writes
    TFile*     m_outputFile;          ///< The output file
    TTree*     m_outputTree;          ///< The output tree
    std::vector<float> m_vx;          ///< Vertex position x
    std::vector<float> m_vy;          ///< Vertex position y
    std::vector<float> m_vz;          ///< Vertex position z
    std::vector<float> m_px;          ///< Momentum position x
    std::vector<float> m_py;          ///< Momentum position y
    std::vector<float> m_pz;          ///< Momentum position z
    std::vector<float> m_pT;          ///< Momentum position transverse component
    std::vector<float> m_eta;         ///< Momentum direction eta
    std::vector<float> m_phi;         ///< Momentum direction phi
    std::vector<float> m_mass;        ///< Particle mass
    std::vector<int>   m_charge;      ///< Particle charge
    std::vector<int>   m_pdgCode;     ///< Particle pdg code
    std::vector<int>   m_barcode;     ///< Particle barcode
    std::vector<int>   m_vertex;      ///< Barcode vertex generation
    std::vector<int>   m_primary;     ///< Barcode primary identifcation
    std::vector<int>   m_generation;  ///< Barcode generation
    std::vector<int>   m_secondary;   ///< Barcode secondary identification
    std::vector<int>   m_process;     ///< Barcode process production
  };

}  // namespace Root
}  // namespace FW
