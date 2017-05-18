#include <iostream>
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"

FWCsv::CsvParticleWriter::CsvParticleWriter(
    const FWCsv::CsvParticleWriter::Config& cfg)
  : FW::IWriterT<std::vector<Acts::ParticleProperties> >()
  , m_cfg(cfg)
{}

FW::ProcessCode
FWCsv::CsvParticleWriter::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWCsv::CsvParticleWriter::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWCsv::CsvParticleWriter::write(const std::vector<Acts::ParticleProperties>& particles)
{
  // abort if you have no stream
  if (!m_cfg.outputStream)   return FW::ProcessCode::ABORT;
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);
    
  (*(m_cfg.outputStream)) << '\n';
  (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
  // loop and fill
  for (auto& particle : particles ){
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

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWCsv::CsvParticleWriter::write(const std::string& sinfo)
{
  // abort if you have no stream
  if (!m_cfg.outputStream)   return FW::ProcessCode::ABORT;
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_write_mutex);
  (*(m_cfg.outputStream)) << sinfo;
  // now return success 
  return FW::ProcessCode::SUCCESS;
}

