#include "ACTFW/Framework/BareAlgorithm.hpp"

FW::BareAlgorithm::BareAlgorithm(std::string name, Acts::Logging::Level level)
  : m_name(std::move(name)), m_logger(Acts::getDefaultLogger(m_name, level))
{
}

std::string
FW::BareAlgorithm::name() const
{
  return m_name;
}
