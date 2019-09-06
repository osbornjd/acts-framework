// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <string>

#include "ACTFW/Framework/IReader.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {
namespace Csv {

  /// Read particles in the TrackML comma-separated-value format.
  ///
  /// This reads one file per event in the configured input directory
  /// and filename. Files are assumed to be named using the following schema
  ///
  ///     event000000001-<inputFilename>
  ///     event000000002-<inputFilename>
  ///
  /// and each line in the file corresponds to one particle. The
  /// input filename can be configured and defaults to `particles.csv`.
  class CsvParticleReader : public IReader
  {
  public:
    struct Config
    {
      /// Which particle collection to read into.
      std::string output;
      /// Where to read input files from.
      std::string inputDir;
      /// Input file name.
      std::string inputFilename = "particles.csv";
    };

    CsvParticleReader(const Config&        cfg,
                      Acts::Logging::Level level = Acts::Logging::INFO);

    std::string
    name() const final override;

    /// Return the number of events.
    size_t
    numEvents() const final override;

    /// Read out data from the input stream.
    ProcessCode
    read(const FW::AlgorithmContext& ctx) final override;

  private:
    Config                              m_cfg;
    size_t                              m_numEvents;
    std::unique_ptr<const Acts::Logger> m_logger;

    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }
  };

}  // namespace Csv
}  // namespace FW
