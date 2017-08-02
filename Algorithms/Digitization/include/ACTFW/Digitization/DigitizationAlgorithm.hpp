//
//  DigitizationAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H
#define ACTFW_ALGORITHMS_DIGITIZATIONALGORITHM_H

#include <cmath>
#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
class PlanarModuleStepper;
}
namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
}  // namespace FW

namespace FWA {

class DigitizationAlgorithm : public FW::BareAlgorithm
{
public:
  struct Config
  {
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    /// input hit collection
    std::string simulatedHitsCollection;
    /// output space point collection
    std::string spacePointCollection;
    /// output clusters collection
    std::string clustersCollection;
    /// module stepper
    std::shared_ptr<Acts::PlanarModuleStepper> planarModuleStepper = nullptr;
  };

  /// Constructor
  DigitizationAlgorithm(const Config&        cnf,
                        Acts::Logging::Level level = Acts::Logging::INFO);

  /// Framework execode method
  FW::ProcessCode
  execute(FW::AlgorithmContext ctx) const final override;

private:
  Config m_cfg;
};

}  // namespace FWA

#endif
