#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"

#include "ACTFW/Framework/WhiteBoard.hpp"

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
  // abort if you have no stream
  if (!m_cfg.outputStream) return ProcessCode::ABORT;

  const std::vector<Acts::ParticleProperties>* particles;
  if (ctx.eventStore.get(m_cfg.collection, particles) != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;

  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);
  (*(m_cfg.outputStream)) << '\n';
  (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
  // loop and fill
  for (auto& particle : (*particles)) {
    // write out the information
    (*(m_cfg.outputStream)) << particle.barcode() << ", [";
    (*(m_cfg.outputStream)) << particle.vertex().x() << ", ";
    (*(m_cfg.outputStream)) << particle.vertex().y() << ", ";
    (*(m_cfg.outputStream)) << particle.vertex().z() << "], [";
    (*(m_cfg.outputStream)) << particle.momentum().mag() << ", ";
    (*(m_cfg.outputStream)) << particle.momentum().theta() << ", ";
    (*(m_cfg.outputStream)) << particle.momentum().phi() << "], ";
    (*(m_cfg.outputStream)) << particle.charge() << '\n';
  }
  (*(m_cfg.outputStream)) << '\n';

  return ProcessCode::SUCCESS;
}
