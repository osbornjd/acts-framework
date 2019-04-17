// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <vector>
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/IReader.hpp"
#include "Acts/Utilities/Logger.hpp"

namespace FW {
  namespace Csv {

    /// Read the particles while are in comma-separated-value format.
    ///
    /// This reads one file per event in the configured input directory 
    /// and filename. By default it reads the file the current working directory.
    /// Files are assumed to be named using the following schema
    ///
    ///     event000000001-particles.csv
    ///     event000000002-particles.csv
    ///
    /// and each line in the file corresponds to one particle.
    /// The file name, e.g. 'particles' could be configured.
    class CsvParticleReader : public IReader 
    {
    public:
      struct Config
      {
        std::string inputDir;                ///< where to place input files
        std::string inputFileName;           ///< input file name
        std::string outputParticleCollection;///< output particle collection 
        size_t nEvents;                      ///< the number of events to be read in (needd for reader interface)
      };
        
      /// constructor
      /// @param cfg is the configuration object
      /// @param level is the output logging level
      CsvParticleReader(const Config&        cfg,
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
      Config                              m_cfg;        ///< Nested configuration struct
      std::unique_ptr<const Acts::Logger> m_logger;     ///< The logging instance 
        
      const Acts::Logger&
      logger() const
      {
        return *m_logger;
      }
        
    };
        
    inline std::string
    CsvParticleReader::name() const
    {
      return "CsvParticleReader";
    }
        
    /// Return of the number events
    inline size_t
    CsvParticleReader::numEvents() const
    {
      return m_cfg.nEvents;
    }
        
  }  // namespace Csv
}  // namespace FW
