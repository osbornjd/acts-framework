#include "HelloWorldAlgorithm.hpp"
#include <iostream>

FWE::HelloWorldAlgorithm::HelloWorldAlgorithm(Acts::Logging::Level level)
  : FW::BareAlgorithm("HelloWorld", level)
{
}

FW::ProcessCode
FWE::HelloWorldAlgorithm::execute(FW::AlgorithmContext context) const
{
  ACTS_INFO(" Hello World! (from event " << context.eventNumber << ")");
  ACTS_DEBUG("  - that's an ACTS_DEBUG message");
  ACTS_VERBOSE("  - that's an ACTS_VERBOSE message");  
  return FW::ProcessCode::SUCCESS;
}
