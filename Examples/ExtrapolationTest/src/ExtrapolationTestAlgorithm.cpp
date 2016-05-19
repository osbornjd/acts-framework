#include "ACTS/Extrapolation/IExtrapolationEngine.h"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Framework/MsgStreamMacros.hpp"
#include "ACTFW/Random/RandomNumbers.hpp"
#include "ExtrapolationTestAlgorithm.hpp"
#include <iostream>

FWE::ExtrapolationTestAlgorithm::ExtrapolationTestAlgorithm(const FWE::ExtrapolationTestAlgorithm::Config& cfg) :
    FW::Algorithm(cfg),
    m_cfg(cfg)
{}

FWE::ExtrapolationTestAlgorithm::~ExtrapolationTestAlgorithm()
{}

/** Framework finalize mehtod */
FW::ProcessCode FWE::ExtrapolationTestAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> eStore,
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
FW::ProcessCode FWE::ExtrapolationTestAlgorithm::execute(size_t eventNumber)
{

    

    return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode FWE::ExtrapolationTestAlgorithm::finalize()
{
    MSG_VERBOSE("initialize successful.");
    return FW::ProcessCode::SUCCESS;
}
