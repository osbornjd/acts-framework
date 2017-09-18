//
//  DigitizationAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H
#define ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H

#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
class PlanarModuleStepper;
}
namespace FW {

class RandomNumbersSvc;

class DigitizationAlgorithm : public FW::BareAlgorithm
{
public:
  struct Config
  {
    /// input hit collection
    std::string simulatedHitsCollection;
    /// output space point collection
    std::string spacePointsCollection;
    /// output clusters collection
    std::string clustersCollection;
    /// FW random number service
    std::shared_ptr<RandomNumbersSvc> randomNumbers = nullptr;
    /// module stepper
    std::shared_ptr<Acts::PlanarModuleStepper> planarModuleStepper = nullptr;
  };

  DigitizationAlgorithm(const Config&        cnf,
                        Acts::Logging::Level level = Acts::Logging::INFO);

  FW::ProcessCode
  execute(FW::AlgorithmContext ctx) const override final;

private:
  Config                              m_cfg;
};

}  // namespace FW

#endif
