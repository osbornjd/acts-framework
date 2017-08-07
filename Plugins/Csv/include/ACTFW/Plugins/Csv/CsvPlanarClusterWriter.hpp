//  CsvPlanarClusterWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H
#define ACTFW_CSV_PLUGINS_PLANARCLUSTERWRITER_H

#include <memory>

#include <ACTS/Digitization/PlanarModuleCluster.hpp>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WriterT.hpp"

namespace FW {
namespace Csv {

  /// @class CsvPlanarClusterWriter
  ///
  /// A root based implementation to write out particleproperties vector
  ///
  class CsvPlanarClusterWriter
    : public WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>
  {
  public:
    using Base = WriterT<DetectorData<geo_id_value, Acts::PlanarModuleCluster>>;
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
    ~CsvPlanarClusterWriter() = default;

  protected:
    ProcessCode
    writeT(const AlgorithmContext& ctx,
           const DetectorData<geo_id_value, Acts::PlanarModuleCluster>&
               clusters) final;

  private:
    Config m_cfg;
  };

}  // namespace Csv
}  // namespace FW

#endif  // ACTFW_PLUGINS_PARTICLEPROPERTIESWRITER_H
