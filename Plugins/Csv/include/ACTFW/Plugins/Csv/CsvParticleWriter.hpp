// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>

#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WriterT.hpp"

namespace FW {
namespace Csv {

  /// Write out particles in the TrackML comma-separated-value format.
  ///
  /// This writer is restricted to outgoing particles, it is designed for
  /// generated particle information.
  ///
  /// This writes one file per event into the configured output directory. By
  /// default it writes to the current working directory. Files are named
  /// using the following schema
  ///
  ///     event000000001-<outputFilename>
  ///     event000000002-<outputFilename>
  ///     ...
  ///
  /// and each line in the file corresponds to one particle.
  class CsvParticleWriter
    : public WriterT<std::vector<Data::SimVertex<Data::SimParticle>>>
  {
  public:
    using Base = WriterT<std::vector<Data::SimVertex<Data::SimParticle>>>;

    struct Config
    {
      /// Input particle collection to write.
      std::string input;
      /// Input collection to map particle ids to number of hits (optional).
      std::string inputHitsPerParticle;
      /// Where to place output files.
      std::string outputDir;
      /// Output file name suffix
      std::string outputFilename = "particles.csv";
      /// Number of decimal digits for floating point precision in output.
      std::size_t outputPrecision = 6;
    };

    /// constructor
    /// @param cfg is the configuration object
    /// @parm level is the output logging level
    CsvParticleWriter(const Config&        cfg,
                      Acts::Logging::Level level = Acts::Logging::INFO);

  protected:
    /// @brief Write method called by the base class
    /// @param [in] context is the algorithm context for consistency
    /// @param [in] vertices is the process vertex collection for the
    /// particles to be attached
    ProcessCode
    writeT(const FW::AlgorithmContext&           context,
           const std::vector<Data::SimVertex<>>& vertices) final override;

  private:
    Config m_cfg;  //!< Nested configuration struct
  };

}  // namespace Csv
}  // namespace FW
