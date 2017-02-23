//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H
#define ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H 1

#include <mutex>

#include <iostream>
#include <fstream>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Writers/IParticlePropertiesWriter.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FWCsv {

/// @class CsvParticleWriter
///
/// A root based implementation to write out particleproperties vector
///
class CsvParticleWriter : public FW::IParticlePropertiesWriter
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
        
    std::shared_ptr<FW::BarcodeSvc>
    barcodeSvc;  ///< the barcode service to decode

    Config(const std::string&   lname = "CsvParticleWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl))
      , name(lname)
      , barcodeSvc(nullptr)
    {
    }
        
  };

  /// Constructor
  ///
  /// @param cfg is the configuration class
  CsvParticleWriter(const Config& cfg);

  /// Destructor
  virtual ~CsvParticleWriter();

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param pProperties is the vector of particle properties
  FW::ProcessCode
  write(const std::vector<Acts::ParticleProperties>& pProperties);

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
CsvParticleWriter::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
