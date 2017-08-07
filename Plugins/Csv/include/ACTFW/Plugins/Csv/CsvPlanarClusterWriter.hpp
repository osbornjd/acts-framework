/// @file
/// @date 2016-05-23 Initial version
/// @date 2017-08-07 Rewrite with new interfaces
/// @autor Andreas Salzburger
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_CSVPLANARCLUSTERWRITER_H
#define ACTFW_CSVPLANARCLUSTERWRITER_H

#include <ACTS/Digitization/PlanarModuleCluster.hpp>
<<<<<<< HEAD
=======

>>>>>>> 365b2f2... csv: convert cluster writer to WriterT
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WriterT.hpp"

namespace FW {
namespace Csv {

  /// Write out a planar cluster collection in comma-separated-value format.
  ///
  /// This writes one file per event into the configured output directory. By
  /// default it writes to the current working directory. Files are named
  /// using the following schema
  ///
<<<<<<< HEAD
  ///     event000000001-hits.csv
  ///     event000000002-hits.csv
  ///
  /// and each line in the file corresponds to one hit/cluster.
=======
>>>>>>> 365b2f2... csv: convert cluster writer to WriterT
  class CsvPlanarClusterWriter
    : public WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>
  {
  public:
    using Base = WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>;
    struct Config
    {
      std::string collection;           ///< which collection to write
      std::string outputDir;            ///< where to place output files
      size_t      outputPrecision = 6;  ///< floating point precision
    };

    /// Constructor with
    /// @param cfg configuration struct 
    /// @param output logging level
    CsvPlanarClusterWriter(const Config&        cfg,
                           Acts::Logging::Level level = Acts::Logging::INFO);
    /// default destructor                       
    ~CsvPlanarClusterWriter() = default;

  protected:
<<<<<<< HEAD
    /// This implementation holds the actual writing method
    /// and is called by the WriterT<>::write interface
=======
>>>>>>> 365b2f2... csv: convert cluster writer to WriterT
    ProcessCode
    writeT(const AlgorithmContext& ctx,
           const DetectorData<geo_id_value, Acts::PlanarModuleCluster>&
               clusters) final;

  private:
    Config m_cfg;
<<<<<<< HEAD
};
=======
  };

>>>>>>> 365b2f2... csv: convert cluster writer to WriterT
}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_CSVPLANARCLUSTERWRITER_H
