// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
#pragma once

#include <map>
#include <memory>
#include <string>

#include "ACTFW/Framework/IReader.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace Acts {
class Surface;
}
namespace FW {
namespace Csv {

  /// Read in a planar cluster collection in comma-separated-value format.
  ///
  /// This reads three files per event file in the configured input
  /// directory. By default it reads file in the current working directory.
  /// Files are assumed to be named using the following schema
  ///
  ///     event000000001-cells.csv
  ///     event000000001-hits.csv
  ///     event000000001-truth.csv
  ///     event000000002-cells.csv
  ///     event000000002-hits.csv
  ///     event000000002-truth.csv
  ///
  /// and each line in the file corresponds to one hit/cluster.
  class CsvPlanarClusterReader : public IReader
  {
  public:
    struct Config
    {
      /// Tracking geometry required to access global-to-local transforms.
      std::shared_ptr<const Acts::TrackingGeometry> trackingGeometry;
      /// Where to write the cluster collection.
      std::string outputClusters;
      /// Where to write the cluster-particle mapping collection.
      std::string outputHitParticleMap;
      /// Where to read input files from.
      std::string inputDir;
    };

    CsvPlanarClusterReader(const Config&        cfg,
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
    Config                                           m_cfg;
    std::map<Acts::GeometryID, const Acts::Surface*> m_surfaces;
    size_t                                           m_numEvents;
    std::unique_ptr<const Acts::Logger>              m_logger;

    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }
  };

}  // namespace Csv
}  // namespace FW
