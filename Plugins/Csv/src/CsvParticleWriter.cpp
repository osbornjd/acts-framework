#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include <fstream>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Csv::CsvParticleWriter::CsvParticleWriter(
    const FW::Csv::CsvParticleWriter::Config& cfg,
    Acts::Logging::Level                      level)
  : Base(cfg.collection, "CsvParticleWriter", level), m_cfg(cfg)
{}

FW::ProcessCode
FW::Csv::CsvParticleWriter::writeT(
    const FW::AlgorithmContext&                  ctx,
    const std::vector<Acts::ProcessVertex>& vertices)
{
  std::string path
      = perEventFilepath(m_cfg.outputDir, "particles.csv", ctx.eventNumber);
  std::ofstream os(path, std::ofstream::out | std::ofstream::trunc);
  if (!os) {
    ACTS_ERROR("Could not open '" << path << "' to write");
  }

  // write csv header
  os << "barcode,";
  os << "vx,vy,vz,";
  os << "px,py,pz,";
  os << "q\n";

  // write one line per particle
  os << std::setprecision(m_cfg.outputPrecision);
  for (auto& vertex: vertices){
    auto& vtx = vertex.position();
    for (auto& particle : vertex.outgoingParticles()) {
      os << particle.barcode() << ",";
      os << vtx.x() << ",";
      os << vtx.y() << ",";
      os << vtx.z() << ",";
      os << particle.momentum().x() << ",";
      os << particle.momentum().y() << ",";
      os << particle.momentum().z() << ",";
      os << particle.charge() << '\n';
    }
  }
  return ProcessCode::SUCCESS;
}
