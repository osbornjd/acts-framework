#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"

#include <fstream>

#include <ACTS/Digitization/PlanarModuleCluster.hpp>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Csv::CsvPlanarClusterWriter::CsvPlanarClusterWriter(
    const FW::Csv::CsvPlanarClusterWriter::Config& cfg,
    Acts::Logging::Level                           level)
  : Base(cfg.collection, "CsvPlanarClusterWriter", level), m_cfg(cfg)
{
}

FW::ProcessCode
FW::Csv::CsvPlanarClusterWriter::writeT(
    const AlgorithmContext&                                          ctx,
    const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>& clusters)
{
  // open per-event hits file
  std::string pathHits
      = perEventFilepath(m_cfg.outputDir, "hits.csv", ctx.eventNumber);
  std::ofstream osHits(pathHits, std::ofstream::out | std::ofstream::trunc);
  if (!osHits) {
    ACTS_ERROR("Could not open '" << pathHits << "' to write");
    return ProcessCode::ABORT;
  }
  // open per-event truth file
  std::string pathTruth
      = perEventFilepath(m_cfg.outputDir, "truth.csv", ctx.eventNumber);
  std::ofstream osTruth(pathTruth, std::ofstream::out | std::ofstream::trunc);
  if (!osTruth) {
    ACTS_ERROR("Could not open '" << pathTruth << "' to write");
    return ProcessCode::ABORT;
  }

  // write csv header
  osHits << "hit_id, ";
  osHits << "volume_id, ";
  osHits << "layer_id, ";
  osHits << "x, y, z, ex, ey, ez, ";
  osHits << "phi, theta, ephi, etheta, ";
  osHits << "fch0, fch1, fvalue\n";
  osHits << std::setprecision(m_cfg.outputPrecision);
  osTruth << "barcode, hit_id\n";

  size_t hitId = 0;
  for (auto& volumeData : clusters) {
    for (auto& layerData : volumeData.second) {
      for (auto& moduleData : layerData.second) {
        for (auto& cluster : moduleData.second) {
          // Identifier @todo replace by identifier
          // (*(m_cfg.outputStream)) << cluster.identifier().value() << ", ";
          hitId += 1;

          // local cluster information
          auto           parameters = cluster.parameters();
          Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0],
                               parameters[Acts::ParDef::eLOC_1]);
          Acts::Vector3D pos(0, 0, 0);
          Acts::Vector3D mom(1, 1, 1);
          // transform local into global position information
          cluster.referenceSurface().localToGlobal(local, mom, pos);

          // write hit information
          osHits << hitId << ", ";
          osHits << volumeData.first << ", ";
          osHits << layerData.first << ", ";
          osHits << pos.x() << ", " << pos.y() << "," << pos.z() << ", ";
          osHits << "-1.0, -1.0, -1.0, ";  // TODO ex, ey, ez
          osHits << pos.phi() << ", " << pos.theta()
                 << ", ";          // TODO phi, theta
          osHits << "-1.0, -1.0";  // TODO ephi, etheta
          // append cell information
          for (auto& cell : cluster.digitizationCells()) {
            osHits << ", " << cell.channel0 << ", " << cell.channel1 << ", "
                   << cell.data;
          }
          osHits << '\n';

          // write hit-particle truth association
          // each hit can have multiple particles, e.g. in a dense environment
          for (auto& barcode : cluster.barcodes()) {
            osTruth << barcode << ", " << hitId << '\n';
          }
        }
      }
    }
  }
  return FW::ProcessCode::SUCCESS;
}
