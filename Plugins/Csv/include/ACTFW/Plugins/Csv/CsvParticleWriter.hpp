//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H
#define ACTFW_CSV_PLUGINS_PARTICLEPROPERTIESWRITER_H

#include <vector>

#include <ACTS/EventData/ParticleDefinitions.hpp>
#include <ACTS/Utilities/Logger.hpp>

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WriterT.hpp"

namespace FW {
namespace Csv {

  /// @class CsvParticleWriter
  ///
  /// A root based implementation to write out particleproperties vector
  ///
  class CsvParticleWriter
    : public WriterT<std::vector<Acts::ParticleProperties>>
  {
  public:
    using Base = WriterT<std::vector<Acts::ParticleProperties>>;
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

  protected:
    ProcessCode
    writeT(const FW::AlgorithmContext&                  ctx,
           const std::vector<Acts::ParticleProperties>& particles) final;

  private:
    Config m_cfg;
  };

}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
