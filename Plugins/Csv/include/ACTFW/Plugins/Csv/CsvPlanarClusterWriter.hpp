// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WriterT.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"

namespace FW {
namespace Csv {

  /// Write out a planar cluster collection in comma-separated-value format.
  ///
  /// This writes multiples file per event containing information about the
  /// space points, local constituent cells, and hit-particle truth mapping
  /// into the configured output directory. By default it writes to the
  /// current working directory. Files are named using the following schema
  ///
  ///     event000000001-cells.csv
  ///     event000000001-hits.csv
  ///     event000000001-truth.csv
  ///     event000000002-cells.csv
  ///     event000000002-hits.csv
  ///     event000000002-truth.csv
  ///     ...
  ///
  /// and each line in the file corresponds to one hit/cluster.
  class CsvPlanarClusterWriter
    : public WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>
  {
  public:
    using Base = WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>;

    struct Config
    {
      /// Which cluster collection to write.
      std::string collection;
      /// Where to place output files
      std::string outputDir;
      /// Number of decimal digits for floating point precision in output.
      size_t outputPrecision = 6;
    };

    /// Constructor with
    /// @param cfg configuration struct
    /// @param output logging level
    CsvPlanarClusterWriter(const Config&        cfg,
                           Acts::Logging::Level level = Acts::Logging::INFO);

  protected:
    /// This implementation holds the actual writing method
    /// and is called by the WriterT<>::write interface
    ProcessCode
    writeT(const AlgorithmContext& context,
           const DetectorData<geo_id_value, Acts::PlanarModuleCluster>&
               clusters) final override;

  private:
    Config m_cfg;
  };
}  // namespace Csv
}  // namespace FW
