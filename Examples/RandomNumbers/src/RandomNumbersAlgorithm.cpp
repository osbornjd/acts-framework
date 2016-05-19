#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include "ACTFW/Random/RandomNumbers.hpp"
#include "RandomNumbersAlgorithm.hpp"
#include <iostream>

FWE::RandomNumbersAlgorithm::RandomNumbersAlgorithm(const FWE::RandomNumbersAlgorithm::Config& cfg) :
    FW::Algorithm(cfg),
    m_cfg(cfg)
{}

FWE::RandomNumbersAlgorithm::~RandomNumbersAlgorithm()
{}

/** Framework finalize mehtod */
FW::ProcessCode FWE::RandomNumbersAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> eStore,
                                                     std::shared_ptr<FW::WhiteBoard> jStore)
{
    // call the algorithm initialize for setting the stores
    if ( FW::Algorithm::initialize(eStore,jStore) != FW::ProcessCode::SUCCESS){
        MSG_FATAL("Algorithm::initialize() did not succeed!");
        return FW::ProcessCode::SUCCESS;
    }
    MSG_VERBOSE("initialize successful.");
    return FW::ProcessCode::SUCCESS;
}

/** Framework execode method */
FW::ProcessCode FWE::RandomNumbersAlgorithm::execute(size_t eventNumber)
{

    
    for (size_t idraw = 0; idraw < m_cfg.drawsPerEvent; ++idraw){

        double gauss   = m_cfg.randomNumbers->draw(FW::Distribution::gauss);
        double uniform = m_cfg.randomNumbers->draw(FW::Distribution::uniform);
        double landau  = m_cfg.randomNumbers->draw(FW::Distribution::landau);
        double gamma   = m_cfg.randomNumbers->draw(FW::Distribution::gamma);

        MSG_VERBOSE("Gauss   : " << gauss );
        MSG_VERBOSE("Uniform : " << uniform );
        MSG_VERBOSE("Landau  : " << landau );
        MSG_VERBOSE("Gamma   : " << gamma );

    }
    return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode FWE::RandomNumbersAlgorithm::finalize()
{
    MSG_VERBOSE("initialize successful.");
    return FW::ProcessCode::SUCCESS;
}
