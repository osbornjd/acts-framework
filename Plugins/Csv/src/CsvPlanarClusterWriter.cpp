#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"

#include <fstream>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
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
  const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>* clusters;
  if (ctx.eventStore.get(m_cfg.collection, clusters) != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;

  // open per-event file
  std::string path
      = perEventFilepath(m_cfg.outputDir, "hits.csv", ctx.eventNumber);
  std::ofstream os(path, std::ofstream::out | std::ofstream::trunc);
  if (!os) {
    ACTS_ERROR("Could not open '" << path << "' to write");
    return ProcessCode::ABORT;
  }

  // now write out
  os << std::setprecision(m_cfg.outputPrecision);

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
            os << ++hitCounter << ", ";
            // contributing barcode
            os << barcode << ", ";
            // local position
            os << "[ " << lx << ", " << ly << "], ";
            // local error
            os << "[ " << ex << ", " << ey << "],";
            // pobal position
            os << "[ " << pos.x() << ", " << pos.y() << "," << pos.z() << "], ";
            // thickness of the cluster
            double thickness = clusterSurface.associatedDetectorElement()
                ? clusterSurface.associatedDetectorElement()->thickness()
                : 0.;
            os << thickness << ",  [";
            // feature set
            size_t cellCounter = 0;
            for (auto& cell : cluster.digitizationCells()) {
              // pobal position
              os << "[ " << cell.channel0 << ", " << cell.channel1 << ", "
                 << cell.data << "]";
              if (cellCounter < cluster.digitizationCells().size() - 1)
                os << ", ";
              ++cellCounter;
            }
            os << "]" << '\n';
          }
        }
  // add a new loine
  os << '\n';
  // return success
  return FW::ProcessCode::SUCCESS;
}
