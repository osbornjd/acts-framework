//
//  RandomNumbersAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H
#define ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H 1

#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
}

namespace FWE {

/// @class Algorithm
class RandomNumbersAlgorithm : public FW::Algorithm
{
public:
  /// @class Config
  struct Config : public FW::Algorithm::Config
  {
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    size_t                                drawsPerEvent = 0;

    Config() : FW::Algorithm::Config("RandomNumbersAlgorithm") {}
  };

  /// Constructor
  RandomNumbersAlgorithm(const Config&                 cnf,
                         std::unique_ptr<Acts::Logger> logger
                         = Acts::getDefaultLogger("RandomNumbersAlgorithm",
                                                  Acts::Logging::INFO));

  /// Destructor
  ~RandomNumbersAlgorithm();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) override final;

  /// Framework execode method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

private:
  Config m_cfg;  ///< the config class
};
}

#endif  // ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H
