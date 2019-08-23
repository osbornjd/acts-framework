// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <Acts/Geometry/TrackingGeometry.hpp>
#include <Acts/Surfaces/Surface.hpp>
#include <Acts/Utilities/Logger.hpp>

#include "ACTFW/Framework/IWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvSurfaceWriter.hpp"

namespace Acts {
class TrackingVolume;
}

namespace FW {
namespace Csv {

  /// @class CsvTrackingGeometryWriter
  ///
  /// An Csv writer for the geometry
  /// It delegates the writing of surfaces to the surface writers
  class CsvTrackingGeometryWriter : public IWriter
  {
  public:
    struct Config
    {
      /// The tracking geometry that should be written.
      const Acts::TrackingGeometry* trackingGeometry = nullptr;
      /// Where to place output files.
      std::string outputDir;
      /// Number of decimal digits for floating point precision in output.
      std::size_t outputPrecision = 6;
      /// the name of the writer
      std::string name = "";
      /// surfaceWriters
      std::shared_ptr<CsvSurfaceWriter> surfaceWriter = nullptr;
      std::string                       filePrefix    = "";
      std::string                       layerPrefix   = "";
    };

    /// Constructor
    /// @param cfg is the configuration class
    CsvTrackingGeometryWriter(const Config&        cfg,
                              Acts::Logging::Level lvl = Acts::Logging::INFO);

    std::string
    name() const override;

    /// Write geometry using the per-event context (optional).
    ProcessCode
    write(const AlgorithmContext& context) override;

    /// Write geometry using the default context.
    ProcessCode
    endRun() override;

    /// The write interface
    /// @param context The algorithm/event context under which this is called
    /// @param tGeometry is the geometry to be written out
    /// @return ProcessCode to indicate success/failure
    FW::ProcessCode
    write(const AlgorithmContext&       context,
          const Acts::TrackingGeometry& tGeometry);

  private:
    Config                              m_cfg;
    const Acts::TrackingVolume*         m_world;
    std::unique_ptr<const Acts::Logger> m_logger;

    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }

    /// process this volume
    /// @param context The algorithm/event context under which this is called
    /// @param tVolume the volume to be processed
    void
    write(const AlgorithmContext& context, const Acts::TrackingVolume& tVolume);
  };

}  // namespace Csv
}  // namespace FW
