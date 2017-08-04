#include <fstream>
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
<<<<<<< HEAD
#include <ACTS/Digitization/PlanarModuleCluster.hpp>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
=======

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
>>>>>>> 5477cb3... csv: convert cluster writer to IWriter

FW::Csv::CsvPlanarClusterWriter::CsvPlanarClusterWriter(
    const FW::Csv::CsvPlanarClusterWriter::Config& cfg,
    Acts::Logging::Level                           level)
<<<<<<< HEAD
  : Base(cfg.collection, "CsvPlanarClusterWriter", level), m_cfg(cfg)
{
}

=======
  : m_cfg(cfg)
  , m_logger(Acts::getDefaultLogger("CsvPlanarClusterWriter", level))
{
}

std::string
FW::Csv::CsvPlanarClusterWriter::name() const
{
  return "CsvPlanarClusterWriter";
}

>>>>>>> 5477cb3... csv: convert cluster writer to IWriter
FW::ProcessCode
FW::Csv::CsvPlanarClusterWriter::writeT(
    const AlgorithmContext&                                          ctx,
    const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>& clusters)
{
<<<<<<< HEAD
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

  // write csv hits header
  osHits << "hit_id,";
  osHits << "volume_id,layer_id,module_id,";
  osHits << "x,y,z,ex,ey,ez,";
  osHits << "phi,theta,ephi,etheta,";
  osHits << "ncells,ch0,ch1,value\n";
  osHits << std::setprecision(m_cfg.outputPrecision);
  // write csv truth headers
  osTruth << "barcode,hit_id\n";

  size_t hitId = 0;
  for (auto& volumeData : clusters) {
    for (auto& layerData : volumeData.second) {
      for (auto& moduleData : layerData.second) {
=======
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Csv::CsvPlanarClusterWriter::finalize()
{
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Csv::CsvPlanarClusterWriter::write(const AlgorithmContext& ctx)
{
  // abort if you have no stream
  if (!m_cfg.outputStream) return FW::ProcessCode::ABORT;

  const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>* clusters;
  if (ctx.eventStore.get(m_cfg.collection, clusters) != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;

  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);
  // now write out
  (*(m_cfg.outputStream)) << '\n';
  (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);

  size_t hitCounter = 0;
  // loop and fill
  for (auto& volumeData : (*clusters))
    for (auto& layerData : volumeData.second)
      for (auto& moduleData : layerData.second)
>>>>>>> 5477cb3... csv: convert cluster writer to IWriter
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
          osHits << hitId << ",";
          osHits << volumeData.first << ",";
          osHits << layerData.first << ",";
          osHits << moduleData.first << ",";
          osHits << pos.x() << "," << pos.y() << "," << pos.z() << ",";
          osHits << "-1.0,-1.0,-1.0,";                       // TODO ex, ey, ez
          osHits << pos.phi() << "," << pos.theta() << ",";  // TODO phi, theta
          osHits << "-1.0,-1.0,";  // TODO ephi, etheta
          // append cell information
          const auto& cells = cluster.digitizationCells();
          osHits << cells.size();
          for (auto& cell : cells) {
            osHits << "," << cell.channel0 << "," << cell.channel1 << ","
                   << cell.data;
          }
          osHits << '\n';
          // write hit-particle truth association
          // each hit can have multiple particles, e.g. in a dense environment
          for (auto& barcode : cluster.barcodes()) {
            osTruth << barcode << "," << hitId << '\n';
          }
        }
<<<<<<< HEAD
      }
    }
  }
=======
  // add a new loine
  (*(m_cfg.outputStream)) << '\n';
  // return success
>>>>>>> 5477cb3... csv: convert cluster writer to IWriter
  return FW::ProcessCode::SUCCESS;
}
