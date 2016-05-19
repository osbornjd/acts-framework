#include "ACTFW/Framework/Algorithm.hpp"

FW::Algorithm::Algorithm(const Config& cnf) :
  m_cfg(cnf)
{}

FW::Algorithm::~Algorithm()
{}

/** Framework intialize method */
FW::ProcessCode FW::Algorithm::initialize(std::shared_ptr<WhiteBoard> eStore,
                                          std::shared_ptr<WhiteBoard> jStore)
{
    m_cfg.eBoard = eStore;
    m_cfg.jBoard = jStore; 
    return ProcessCode::SUCCESS;
}

/** Framework execode method */
FW::ProcessCode FW::Algorithm::execute(size_t)
{
    return ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode FW::Algorithm::finalize()
{
    return ProcessCode::SUCCESS;
}



