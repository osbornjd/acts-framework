//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H
#define ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H 1

#include <fstream>
#include <memory>
#include <mutex>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/IWriter.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FWCsv {

/// @class CsvParticleWriter
///
/// A root based implementation to write out particleproperties vector
///
class CsvParticleWriter : public FW::IWriter
{
public:
  struct Config
  {
    /// which collection to write to disk
    std::string collection;
    /// the precision
    size_t outputPrecision = 4;
    /// the ofstream
    std::shared_ptr<std::ofstream> outputStream = nullptr;
    /// the barcode service to decode
    std::shared_ptr<FW::BarcodeSvc> barcodeSvc;
  };

  CsvParticleWriter(const Config&        cfg,
                    Acts::Logging::Level level = Acts::Logging::INFO);
  ~CsvParticleWriter() = default;

  /// Framework name() method
  /// @return the name of the tool
  std::string
  name() const final;

  /// Framework intialize method
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  finalize() final;

  /// The write interface
  /// @param pProperties is the vector of particle properties
  /// @return ProcessCode to indicate success/failure
  FW::ProcessCode
  write(const FW::AlgorithmContext& ctx) final;

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

}  // namespace FWCsv

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
