#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"

#include <fstream>

#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Csv::CsvParticleWriter::CsvParticleWriter(
    const FW::Csv::CsvParticleWriter::Config& cfg,
    Acts::Logging::Level                      level)
  : m_cfg(cfg), m_logger(Acts::getDefaultLogger("CsvParticleWriter", level))
{
}

std::string
FW::Csv::CsvParticleWriter::name() const
{
  return "CsvParticleWriter";
}

FW::ProcessCode
FW::Csv::CsvParticleWriter::initialize()
{
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Csv::CsvParticleWriter::finalize()
{
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Csv::CsvParticleWriter::write(const FW::AlgorithmContext& ctx)
{
  std::string path
      = perEventFilepath(m_cfg.outputDir, "particles.csv", ctx.eventNumber);
  std::ofstream os(path, std::ofstream::out | std::ofstream::trunc);
  if (!os) {
    ACTS_ERROR("Could not open '" << path << "' to write");
    return ProcessCode::ABORT;
  }

  const std::vector<Acts::ParticleProperties>* particles;
  if (ctx.eventStore.get(m_cfg.collection, particles) != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;

  // write one line per particle
  os << std::setprecision(m_cfg.outputPrecision);
  for (auto& particle : (*particles)) {
    os << particle.barcode() << ", [";
    os << particle.vertex().x() << ", ";
    os << particle.vertex().y() << ", ";
    os << particle.vertex().z() << "], [";
    os << particle.momentum().mag() << ", ";
    os << particle.momentum().theta() << ", ";
    os << particle.momentum().phi() << "], ";
    os << particle.charge() << '\n';
  }

  return ProcessCode::SUCCESS;
}
