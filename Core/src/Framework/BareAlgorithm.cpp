#include "ACTFW/Framework/BareAlgorithm.hpp"

FW::BareAlgorithm::BareAlgorithm(std::string name, Acts::Logging::Level level)
  : m_name(std::move(name)), m_logger(Acts::getDefaultLogger(m_name, level))
{
}

FW::BareAlgorithm::~BareAlgorithm()
{
}

std::string
FW::BareAlgorithm::name() const
{
  return m_name;
}

FW::ProcessCode
FW::BareAlgorithm::initialize()
{
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::BareAlgorithm::finalize()
{
  return ProcessCode::SUCCESS;
}
