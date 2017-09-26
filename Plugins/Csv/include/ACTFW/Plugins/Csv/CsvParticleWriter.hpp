/// @file
/// @date 2016-05-23 Initial version
/// @date 2017-08-07 Rewrite with new interfaces
/// @autor Andreas Salzburger
/// @author Moritz Kiehn <msmk@cern.ch>

#ifndef ACTFW_CSVPARTICLERITER_H
#define ACTFW_CSVPARTICLERITER_H
#include <vector>
#include <ACTS/EventData/ParticleDefinitions.hpp>
#include <ACTS/Utilities/Logger.hpp>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WriterT.hpp"

namespace FW {
namespace Csv {

  /// Write out a the particles associated to a list of process vertices,
  /// the particles are in comma-separated-value format.
  ///
  /// This writer is restricted to outgoing particles, it is designed for
  /// generated particle information.
  ///
  /// This writes one file per event into the configured output directory. By
  /// default it writes to the current working directory. 
  /// Files are named using the following schema
  ///
  ///     event000000001-particles.csv
  ///     event000000002-particles.csv
  ///
  /// and each line in the file corresponds to one particle.
  class CsvParticleWriter
    : public WriterT<std::vector<Acts::ProcessVertex>>
  {
  public:
    using Base = WriterT<std::vector<Acts::ProcessVertex>>;

    struct Config
    {
      std::string collection;           ///< which collection to write
      std::string outputDir;            ///< where to place output files
      size_t      outputPrecision = 6;  ///< floating point precision
      /// the barcode service to decode/endcode barcode 
      std::shared_ptr<FW::BarcodeSvc> barcodeSvc;
    };

    /// constructor 
    /// @param cfg is the configuration object
    /// @parm level is the output logging level
    CsvParticleWriter(const Config&        cfg,
                      Acts::Logging::Level level = Acts::Logging::INFO);

  protected:
    ProcessCode
    writeT(const FW::AlgorithmContext& ctx,
           const std::vector<Acts::ProcessVertex>& particles) final override;

  private:
    Config m_cfg;
  };

}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_CSVPARTICLERITER_H
