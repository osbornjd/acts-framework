#include "ACTFW/Framework/Algorithm.hpp"

FW::Algorithm::Algorithm(const Config&                 cfg,
                         std::unique_ptr<Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

FW::Algorithm::~Algorithm()
{
}

FW::ProcessCode
FW::Algorithm::initialize(std::shared_ptr<WhiteBoard> eStore,
                          std::shared_ptr<WhiteBoard> jStore)
{
  m_cfg.eBoard = eStore;
  m_cfg.jBoard = jStore;
  return ProcessCode::SUCCESS;
}

FW::ProcessCode FW::Algorithm::execute(size_t)
{
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Algorithm::finalize()
{
  return ProcessCode::SUCCESS;
}
