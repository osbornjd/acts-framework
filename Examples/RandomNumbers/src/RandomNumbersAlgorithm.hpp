//
//  RandomNumbersAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H
#define ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H 1

#include <array>
#include <memory>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
}  // namespace FW

namespace FWE {

class RandomNumbersAlgorithm : public FW::BareAlgorithm
{
public:
  struct Config
  {
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;

    std::array<double, 2> gaussParameters   = {{0., 1.}};
    std::array<double, 2> uniformParameters = {{0., 1.}};
    std::array<double, 2> landauParameters  = {{0., 1.}};
    std::array<double, 2> gammaParameters   = {{0., 1.}};
    int                   poissonParameter  = 40;

    size_t drawsPerEvent = 0;
  };

  /// Constructor
  RandomNumbersAlgorithm(const Config& cnf);

  /// Framework execode method
  FW::ProcessCode
  execute(FW::AlgorithmContext context) const final override;

private:
  Config m_cfg;
};

}  // namespace FWE

#endif  // ACTFW_EXAMPLES_RANDOMNUMBERSALGORITHM_H
