// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include "ACTFW/Framework/WriterT.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Fatras/Kernel/Particle.hpp"

namespace FW {
namespace Csv {

  /// Write out a the particles associated to a list of process vertices,
  /// the particles are in comma-separated-value format.
  ///
  /// This writer is restricted to outgoing particles, it is designed for
  /// generated particle information.
  ///
  /// This writes one file per event into the configured output directory. By
  /// default it writes to the current working directory.
  /// Files are named using the following schema
  ///
  ///     event000000001-particles.csv
  ///     event000000002-particles.csv
  ///
  /// and each line in the file corresponds to one particle.
  class CsvParticleWriter : public WriterT<std::vector<Fatras::Vertex>>
  {
  public:
    using Base = WriterT<std::vector<Fatras::Vertex>>;

    struct Config
    {
      std::string collection;           ///< which collection to write
      std::string outputDir;            ///< where to place output files
      std::string outputFileName;       ///< output file name
      size_t      outputPrecision = 6;  ///< floating point precision

      /// try to get the hits per particle map
      std::string hitsPerParticleCollection = "";
    };

    /// constructor
    /// @param cfg is the configuration object
    /// @parm level is the output logging level
    CsvParticleWriter(const Config&        cfg,
                      Acts::Logging::Level level = Acts::Logging::INFO);

  protected:
    ProcessCode
    writeT(const FW::AlgorithmContext&        ctx,
           const std::vector<Fatras::Vertex>& vertices) final override;

  private:
    Config m_cfg;
  };

}  // namespace Csv
}  // namespace FW
