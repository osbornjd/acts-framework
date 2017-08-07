//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H
#define ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H

#include <memory>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/IWriter.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
namespace Csv {

  /// @class CsvParticleWriter
  ///
  /// A root based implementation to write out particleproperties vector
  ///
  class CsvParticleWriter : public IWriter
  {
  public:
    struct Config
    {
      std::string collection;           ///< which collection to write
      std::string outputDir;            ///< where to place output files
      size_t      outputPrecision = 4;  ///< floating point precision
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
    ProcessCode
    initialize() final;

    /// Framework finalize mehtod
    /// @return ProcessCode to indicate success/failure
    ProcessCode
    finalize() final;

    /// The write interface
    /// @param pProperties is the vector of particle properties
    /// @return ProcessCode to indicate success/failure
    ProcessCode
    write(const FW::AlgorithmContext& ctx) final;

  private:
    Config                              m_cfg;
    std::unique_ptr<const Acts::Logger> m_logger;

    const Acts::Logger&
    logger() const
    {
      return *m_logger;
    }
  };

}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
