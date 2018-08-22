// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <mutex>
#include "ACTFW/Framework/WriterT.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "TFile.h"
#include "TTree.h"

namespace FW {

namespace Root {

  using PropagationSteps = std::vector<Acts::detail::Step>;

  /// @class RootPropagationStepsWriter
  ///
  /// Write out the steps of test propgations for stepping validation
  class RootPropagationStepsWriter
      : public WriterT<std::vector<PropagationSteps>>
  {
  public:
    using Base = WriterT<std::vector<PropagationSteps>>;
    struct Config
    {
      std::string collection
          = "propagation_steps";          ///< particle collection to write
      std::string filePath = "";          ///< path of the output file
      std::string fileMode = "RECREATE";  ///< file access mode
      std::string treeName = "propagation_steps";  ///< name of the output tree
    };

    /// Constructor with
    /// @param cfg configuration struct
    /// @param output logging level
    RootPropagationStepsWriter(const Config&        cfg,
                               Acts::Logging::Level level
                               = Acts::Logging::INFO);

    /// Virtual destructor
    ~RootPropagationStepsWriter() override;

    /// End-of-run hook
    ProcessCode
    endRun() final override;

  protected:
    /// This implementation holds the actual writing method
    /// and is called by the WriterT<>::write interface
    ProcessCode
    writeT(const AlgorithmContext&              ctx,
           const std::vector<PropagationSteps>& steps) final override;

  private:
    Config     m_cfg;         ///< the configuration object
    std::mutex m_writeMutex;  ///< protect multi-threaded writes
    TFile*     m_outputFile;  ///< the output file

    std::vector<int>   m_volumeID;     ///< volume identifier
    std::vector<int>   m_boundaryID;   ///< boundary identifier
    std::vector<int>   m_layerID;      ///< layer identifier if
    std::vector<int>   m_approachID;   ///< surface identifier
    std::vector<int>   m_sensitiveID;  ///< surface identifier
    std::vector<float> m_x;            ///< global x
    std::vector<float> m_y;            ///< global y
    std::vector<float> m_z;            ///< global z
    std::vector<float> m_dx;           ///< global direction x
    std::vector<float> m_dy;           ///< global direction y
    std::vector<float> m_dz;           ///< global direction z
    std::vector<int>   m_step_type;    ///< step type
    std::vector<float> m_step_acc;     ///< accuracy
    std::vector<float> m_step_act;     ///< actor
    std::vector<float> m_step_abt;     ///< aborter
    std::vector<float> m_step_usr;     ///< user
  };

}  // namespace Root
}  // namespace FW
