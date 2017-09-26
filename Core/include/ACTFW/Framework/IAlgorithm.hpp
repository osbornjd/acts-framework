/// @file
/// @date 2016-05-11 Initial version
/// @author Andreas Salzburger
/// @author Moritz Kiehn <msmk@cern.ch>

#ifndef ACTFW_IALGORITHM_H
#define ACTFW_IALGORITHM_H

#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// Interface for event processing algorithms.
class IAlgorithm
{
public:
  /// Virtual destructor
  virtual ~IAlgorithm() = default;

  /// Framework name() method
  virtual std::string
  name() const = 0;

  /// Framework execute method
  virtual ProcessCode
  execute(AlgorithmContext context) const = 0;
};

}  // namespace FW

#endif  // ACTFW_IALGORITHM_H
