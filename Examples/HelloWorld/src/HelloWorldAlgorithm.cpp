#include "HelloWorldAlgorithm.hpp"

#include <iostream>

FWE::HelloWorldAlgorithm::HelloWorldAlgorithm(const FW::Algorithm::Config& cnf)
  : FW::Algorithm(cnf)
{
}

FWE::HelloWorldAlgorithm::~HelloWorldAlgorithm()
{
}

FW::ProcessCode
FWE::HelloWorldAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::HelloWorldAlgorithm::execute(const FW::AlgorithmContext context) const
{
  ACTS_INFO(" Hello World! (from event " << context.eventContext->eventNumber << ")");
  return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode
FWE::HelloWorldAlgorithm::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}
