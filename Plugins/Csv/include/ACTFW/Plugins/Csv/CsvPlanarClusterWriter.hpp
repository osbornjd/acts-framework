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

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IEventDataWriterT.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
namespace Csv {

  /// @class CsvPlanarClusterWriter
  ///
  /// A root based implementation to write out particleproperties vector
  ///
  class CsvPlanarClusterWriter
    : public FW::IEventDataWriterT<Acts::PlanarModuleCluster>
  {
  public:
    // @class Config
    //
    // The nested config class
    class Config
    {
    public:
      std::shared_ptr<const Acts::Logger> logger;  /// the default logger
      std::string                         name;    /// the name of the algorithm
      size_t                              outputPrecision = 4;
      std::shared_ptr<std::ofstream>      outputStream    = nullptr;

      Config(const std::string&   lname = "CsvPlanarClusterWriter",
             Acts::Logging::Level lvl   = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl)), name(lname)
      {
      }
    };

    /// Constructor
    ///
    /// @param cfg is the configuration class
    CsvPlanarClusterWriter(const Config& cfg);

    /// Framework name() method
    std::string
    name() const final;

    /// Destructor
    virtual ~CsvPlanarClusterWriter();

    /// Framework intialize method
    /// @return ProcessCode to indicate success/failure
    FW::ProcessCode
    initialize() override final;

    /// Framework finalize mehtod
    /// @return ProcessCode to indicate success/failure
    FW::ProcessCode
    finalize() override final;

    /// The write interface
    /// @param pClusters is the DetectorData of planar clusters
    /// @return ProcessCode to indicate success/failure
    FW::ProcessCode
    write(const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>&
              pClusters) override final;

    /// write a bit of string
    /// @param sinfo is some string info to be written
    /// @return is a ProcessCode indicating return/failure
    FW::ProcessCode
    write(const std::string& sinfo) override final;

  private:
    Config     m_cfg;          ///< the config class
    std::mutex m_write_mutex;  ///< mutex used to protect multi-threaded writes

    /// Private access to the logging instance
    const Acts::Logger&
    logger() const
    {
      return *m_cfg.logger;
    }
  };

}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
