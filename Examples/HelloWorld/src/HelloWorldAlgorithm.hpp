#ifndef ACTFW_EXAMPLES_HELLOWORLD_H
#define ACTFW_EXAMPLES_HELLOWORLD_H 1

#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FWE {

/// @class Algorithm
class HelloWorldAlgorithm : public FW::BareAlgorithm
{
public:
  /// Constructor
  HelloWorldAlgorithm();

  /// Framework execode method
  FW::ProcessCode
  execute(FW::AlgorithmContext context) const final override;
};

}  // namespace FWE

#endif  // ACTFW_EXAMPLES_HELLOWORLD_H
