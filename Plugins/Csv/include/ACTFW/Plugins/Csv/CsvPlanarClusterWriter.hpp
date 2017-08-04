//  CsvPlanarClusterWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H
#define ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H

#include <fstream>
#include <memory>
#include <mutex>

#include "ACTFW/Framework/IWriter.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
namespace Csv {

  /// @class CsvPlanarClusterWriter
  ///
  /// A root based implementation to write out particleproperties vector
  ///
  class CsvPlanarClusterWriter : public IWriter
  {
  public:
    struct Config
    {
      // input planar cluster collection
      std::string                    collection;
      size_t                         outputPrecision = 4;
      std::shared_ptr<std::ofstream> outputStream    = nullptr;
    };

    /// Constructor
    ///
    /// @param cfg is the configuration class
    CsvPlanarClusterWriter(const Config&        cfg,
                           Acts::Logging::Level level = Acts::Logging::INFO);

    std::string
    name() const final;

    ProcessCode
    initialize() final;

    ProcessCode
    finalize() final;

    ProcessCode
    write(const AlgorithmContext& ctx) final;

  private:
    Config     m_cfg;          ///< the config class
    std::mutex m_write_mutex;  ///< mutex used to protect multi-threaded writes
    std::unique_ptr<const Acts::Logger> m_logger;

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }
  };

}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
