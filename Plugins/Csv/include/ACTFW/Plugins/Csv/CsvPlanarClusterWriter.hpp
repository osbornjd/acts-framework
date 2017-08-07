//  CsvPlanarClusterWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H
#define ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H

#include <memory>

#include "ACTFW/Framework/IWriter.hpp"
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
      std::string collection;           ///< which collection to write
      std::string outputDir;            ///< where to place output files
      size_t      outputPrecision = 4;  ///< floating point precision
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
    Config                              m_cfg;
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
