//  CsvPlanarClusterWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H
#define ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H 1

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Writers/IPlanarClusterWriter.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FWCsv {

/// @class CsvPlanarClusterWriter
///
/// A root based implementation to write out particleproperties vector
///
class CsvPlanarClusterWriter : public FW::IPlanarClusterWriter
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger>  logger;           ///< the default logger
    std::string                    name;             ///< the name of the algorithm
    size_t                         outputPrecision   = 4;
    std::shared_ptr<std::ofstream> outputStream      = nullptr;

    Config(const std::string&   lname = "CsvPlanarClusterWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
    {
    }
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  CsvPlanarClusterWriter(const Config& cfg);

  /// Destructor
  virtual ~CsvPlanarClusterWriter();

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param pClusters is the DetectorData of planar clusters
  FW::ProcessCode
  write(const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>& pClusters) final;

  /// Framework name() method
  const std::string&
  name() const final;

private:
  Config                             m_cfg;         ///< the config class

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

const std::string&
CsvPlanarClusterWriter::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
