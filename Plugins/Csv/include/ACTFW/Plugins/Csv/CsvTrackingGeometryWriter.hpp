// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <fstream>
#include <iostream>
#include <mutex>

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
    // @class Config
    //
    // The nested config class
    class Config
    {
    public:
      /// the default logger
      std::shared_ptr<const Acts::Logger> logger;
      /// the name of the writer
      std::string name = "";
      /// surfaceWriters
      std::shared_ptr<CsvSurfaceWriter> surfaceWriter = nullptr;
      std::string                       filePrefix    = "";
      std::string                       layerPrefix   = "";

      /// Constructor for the nested config class
      /// @param lname is the name of the writer
      /// @lvl is the screen output logging level
      Config(const std::string&   lname = "CsvTrackingGeometryWriter",
             Acts::Logging::Level lvl   = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl))
        , name(lname)
        , surfaceWriter()
      {
      }
    };

    /// Constructor
    /// @param cfg is the configuration class
    CsvTrackingGeometryWriter(const Config& cfg);

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
    Config m_cfg;  ///< the config class

    /// process this volume
    /// @param context The algorithm/event context under which this is called
    /// @param tVolume the volume to be processed
    void
    write(const AlgorithmContext& context, const Acts::TrackingVolume& tVolume);

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

}  // namespace Csv
}  // namespace FW
