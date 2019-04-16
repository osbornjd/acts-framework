// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "Acts/Plugins/Digitization/PlanarModuleCluster.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/IReader.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"

namespace FW {
  namespace Csv {
        
    /// Read a planar cluster collection in comma-separated-value format.
    ///
    /// This reads three files per event file in the configured input
    /// directory. By default it reads file in the current working directory. 
    /// Files are assumed to be named using the following schema
    ///
    ///     event000000001-hits.csv
    ///     event000000002-hits.csv
    ///     event000000001-details.csv
    ///     event000000002-details.csv
    ///     event000000001-truth.csv
    ///     event000000002-truth.csv
    ///
    /// and each line in the file corresponds to one hit/cluster.
    /// The file name, e.g. 'hits', 'details' and 'truth' could be configured.
    class CsvPlanarClusterReader : public IReader
    {
    public:
        
      struct Config
      {
        std::string inputDir;                                              ///< where to find input files
        std::string inputHitsFileName;                                     ///< name of input hits file, e.g. 'hits'
        std::string inputDetailsFileName;                                  ///< name of input hit details file, e.g. 'details'
        std::string inputTruthFileName;                                    ///< name of input truth file, e.g. 'truth'
        std::string outputClusterCollection;                               ///< name of output cluster collection 
        std::shared_ptr<const Acts::TrackingGeometry> tGeometry = nullptr; ///< trackingGeometry to navigate to a surface
        size_t nEvents;                                                    ///< the number of events to be read in (needd for reader interface)
      };
        
      /// Constructor with
      /// @param cfg configuration struct
      /// @param output logging level
      CsvPlanarClusterReader(const Config&        cfg,
                             Acts::Logging::Level level = Acts::Logging::INFO);
        
      /// Framework name() method
      std::string
      name() const final override;
        
      /// Skip a few events in the IO stream
      /// @param [in] nEvents is the number of skipped events
      ProcessCode
      skip(size_t nEvents) final override;
        
      /// Read out data from the input stream
      ProcessCode
      read(FW::AlgorithmContext ctx) final override;
        
      /// Return the number of events
      virtual size_t
      numEvents() const final override;
        
    private:
      Config  m_cfg;                                  ///< nested configuration struct
      std::unique_ptr<const Acts::Logger>  m_logger;  ///< the logger instance
        
      const Acts::Logger&
      logger() const
      {
        return *m_logger;
      }
        
    };
        
    inline std::string
    CsvPlanarClusterReader::name() const
    {
      return "CsvPlanarClusterReader";
    }
        
    /// Return of the number events
    inline size_t
    CsvPlanarClusterReader::numEvents() const
    {
      return m_cfg.nEvents;
    }
        
  }  // namespace Csv
}  // namespace FW
