#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"

FW::Csv::CsvPlanarClusterWriter::CsvPlanarClusterWriter(
    const FW::Csv::CsvPlanarClusterWriter::Config& cfg,
    Acts::Logging::Level                           level)
  : m_cfg(cfg)
  , m_logger(Acts::getDefaultLogger("CsvPlanarClusterWriter", level))
{
}

std::string
FW::Csv::CsvPlanarClusterWriter::name() const
{
  return "CsvPlanarClusterWriter";
}

FW::ProcessCode
FW::Csv::CsvPlanarClusterWriter::initialize()
{
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
        for (auto& cluster : moduleData.second) {
          // get the global position
          Acts::Vector3D pos(0., 0., 0.);
          Acts::Vector3D mom(1., 1., 1.);
          auto           parameters = cluster.parameters();
          double         lx         = parameters[Acts::ParDef::eLOC_0];
          double         ly         = parameters[Acts::ParDef::eLOC_1];
          double         ex         = 0.;
          double         ey         = 0.;
          Acts::Vector2D local(lx, ly);
          // get the surface
          const Acts::Surface& clusterSurface = cluster.referenceSurface();
          // transform global to local
          clusterSurface.localToGlobal(local, mom, pos);
          // write one line per barcode
          for (auto& barcode : cluster.barcodes()) {
            // write out the data
            // Identifier @todo replace by identifier
            // (*(m_cfg.outputStream)) << cluster.identifier().value() << ", ";
            (*(m_cfg.outputStream)) << ++hitCounter << ", ";
            // contributing barcode
            (*(m_cfg.outputStream)) << barcode << ", ";
            // local position
            (*(m_cfg.outputStream)) << "[ " << lx << ", " << ly << "], ";
            // local error
            (*(m_cfg.outputStream)) << "[ " << ex << ", " << ey << "],";
            // pobal position
            (*(m_cfg.outputStream)) << "[ " << pos.x() << ", " << pos.y() << ","
                                    << pos.z() << "], ";
            // thickness of the cluster
            double thickness = clusterSurface.associatedDetectorElement()
                ? clusterSurface.associatedDetectorElement()->thickness()
                : 0.;
            (*(m_cfg.outputStream)) << thickness << ",  [";
            // feature set
            size_t cellCounter = 0;
            for (auto& cell : cluster.digitizationCells()) {
              // pobal position
              (*(m_cfg.outputStream))
                  << "[ " << cell.channel0 << ", " << cell.channel1 << ", "
                  << cell.data << "]";
              if (cellCounter < cluster.digitizationCells().size() - 1)
                (*(m_cfg.outputStream)) << ", ";
              ++cellCounter;
            }
            (*(m_cfg.outputStream)) << "]" << '\n';
          }
        }
  // add a new loine
  (*(m_cfg.outputStream)) << '\n';
  // return success
  return FW::ProcessCode::SUCCESS;
}
