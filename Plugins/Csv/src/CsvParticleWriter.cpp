#include <iostream>
#include "ACTFW/Csv/CsvParticleWriter.hpp"

FWCsv::CsvParticleWriter::CsvParticleWriter(
    const FWCsv::CsvParticleWriter::Config& cfg)
  : FW::IParticlePropertiesWriter()
  , m_cfg(cfg)
{}

FWCsv::CsvParticleWriter::~CsvParticleWriter()
{
}

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
FWCsv::CsvParticleWriter::write(const std::vector<Acts::ParticleProperties>& pProperties)
{
  if (!m_cfg.outputStream)   return FW::ProcessCode::SUCCESS;
    
  (*(m_cfg.outputStream)) << std::endl;
  (*(m_cfg.outputStream)) << std::setprecision(m_cfg.outputPrecision);
  // loop and fill
  for (auto& particle : pProperties){
    (*(m_cfg.outputStream)) << particle.barcode() << ", [";
    (*(m_cfg.outputStream)) << particle.vertex().x() << ", ";
    (*(m_cfg.outputStream)) << particle.vertex().y() << ", ";
    (*(m_cfg.outputStream)) << particle.vertex().z() << "], [";  
    (*(m_cfg.outputStream)) << particle.momentum().mag() << ", ";  
    (*(m_cfg.outputStream)) << particle.momentum().theta() << ", ";
    (*(m_cfg.outputStream)) << particle.momentum().phi() << "]"<< std::endl;
  }  
  (*(m_cfg.outputStream)) << std::endl;

  return FW::ProcessCode::SUCCESS;
}


