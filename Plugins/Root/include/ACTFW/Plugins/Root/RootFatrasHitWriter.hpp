// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <Fatras/Kernel/Definitions.hpp>
#include <mutex>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "TFile.h"
#include "TTree.h"

namespace FW {

namespace Root {

  /// @class RootFatrasHitWriter
  ///
  /// Write out a planar cluster collection into a root file
  /// to avoid immense long vectors, each cluster is one entry
  /// in the root file
  class RootFatrasHitWriter
      : public WriterT<DetectorData<geo_id_value, Fatras::SensitiveHit>>
  {
  public:
    using Base = WriterT<DetectorData<geo_id_value, Fatras::SensitiveHit>>;
    struct Config
    {
      std::string collection;                ///< cluster collection to write
      std::string filePath;                  ///< path of the output file
      std::string fileMode = "RECREATE";     ///< file access mode
      std::string treeName = "fatras_hits";  ///< name of the output tree
    };

    /// Constructor with
    /// @param cfg configuration struct
    /// @param output logging level
    RootFatrasHitWriter(const Config&        cfg,
                        Acts::Logging::Level level = Acts::Logging::INFO);

    /// Virtual destructor
    ~RootFatrasHitWriter() override;

    /// End-of-run hook
    ProcessCode
    endRun() final override;

  protected:
    /// This implementation holds the actual writing method
    /// and is called by the WriterT<>::write interface
    ProcessCode
    writeT(const AlgorithmContext& ctx,
           const DetectorData<geo_id_value, Fatras::SensitiveHit>& clusters)
        final override;

  private:
    Config     m_cfg;         ///< the configuration object
    std::mutex m_writeMutex;  ///< protect multi-threaded writes
    TFile*     m_outputFile;  ///< the output file

    int   m_volumeID;   ///< volume identifier
    int   m_layerID;    ///< layer identifier
    int   m_surfaceID;  ///< surface identifier
    float m_x;          ///< global x
    float m_y;          ///< global y
    float m_z;          ///< global z
    float m_dx;         ///< global direction x
    float m_dy;         ///< global direction y
    float m_dz;         ///< global direction z
    float m_value;      ///< value of the hit
  };

}  // namespace Root
}  // namespace FW
