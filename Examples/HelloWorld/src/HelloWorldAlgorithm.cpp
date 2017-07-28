#include "HelloWorldAlgorithm.hpp"

#include <iostream>

FWE::HelloWorldAlgorithm::HelloWorldAlgorithm()
  : FW::BareAlgorithm("HelloWorld")
{
}

FW::ProcessCode
FWE::HelloWorldAlgorithm::execute(FW::AlgorithmContext context) const
{
  ACTS_INFO(" Hello World! (from event " << context.eventNumber << ")");
  return FW::ProcessCode::SUCCESS;
}
