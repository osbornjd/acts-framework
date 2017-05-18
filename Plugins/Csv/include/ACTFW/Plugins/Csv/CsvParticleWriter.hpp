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
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TFile;

namespace FWCsv {

/// @class CsvParticleWriter
///
/// A root based implementation to write out particleproperties vector
///
class CsvParticleWriter 
  : public FW::IWriterT<std::vector<Acts::ParticleProperties> > 
{
public:
  // @class Config
  //
  // The nested config class
  class Config
  {
  public:
    std::shared_ptr<Acts::Logger>  logger;                      ///< the default logger
    std::string                    name;                        ///< the name of the algorithm
    size_t                         outputPrecision   = 4;       ///< the precision 
    std::shared_ptr<std::ofstream> outputStream      = nullptr; //!< the ofstream 
        
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
  /// @param cfg is the configuration class
  CsvParticleWriter(const Config& cfg);

  /// Destructor
  virtual ~CsvParticleWriter() = default;

  /// Framework intialize method
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  finalize() override final;

  /// The write interface
  /// @param pProperties is the vector of particle properties
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const std::vector<Acts::ParticleProperties>& particles) override final;

  /// The write interace for string
  /// @param sinfo is some additional string info
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const std::string& sinfo) override final;

  /// Framework name() method
  /// @return the name of the tool
  const std::string&
  name() const final;

private:
  Config        m_cfg;         ///< the config class
  std::mutex    m_write_mutex; ///< mutex used to protect multi-threaded writes
  
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
