//
//  DigitizationAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_EXAMPLES_DIGITIZATIONALGORITHM_H
#define ACTFW_EXAMPLES_DIGITIZATIONALGORITHM_H 1

#include <cmath>
#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
class IExtrapolationEngine;
}

namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
class IExtrapolationCellWriter;
}

namespace FWE {

/// @class Algorithm
class DigitizationAlgorithm : public FW::Algorithm
{
public:
  /// @class Config
  struct Config : public FW::Algorithm::Config
  {
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;

    Config() : FW::Algorithm::Config("DigitizationAlgorithm") {}
  };

  /// Constructor
  DigitizationAlgorithm(
      const Config&                 cnf,
      std::unique_ptr<Acts::Logger> logger
      = Acts::getDefaultLogger("DigitizationAlgorithm",
                               Acts::Logging::INFO));

  /// Destructor
  ~DigitizationAlgorithm();

  /// Framework intialize method
  virtual FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;

  /// Framework execode method
  virtual FW::ProcessCode
  execute(const FW::AlgorithmContext context) const final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

private:
  Config m_cfg;  //!< the config class

};

}

#endif
