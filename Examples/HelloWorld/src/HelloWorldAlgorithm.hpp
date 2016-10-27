#ifndef ACTFW_EXAMPLES_HELLOWORLD_H
#define ACTFW_EXAMPLES_HELLOWORLD_H 1

#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {
class WhiteBoard;
}

namespace FWE {

/// @class Algorithm
class HelloWorldAlgorithm : public FW::Algorithm
{
public:
  /// Constructor
  HelloWorldAlgorithm(const FW::Algorithm::Config& cnf);

  /// Destructor
  ~HelloWorldAlgorithm();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) override final;

  /// Framework execode method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;
};
}

#endif  // ACTFW_EXAMPLES_HELLOWORLD_H
