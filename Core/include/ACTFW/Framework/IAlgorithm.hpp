/// @file
/// @date 2016-05-11 Initial version
/// @author Andreas Salzburger
/// @author Moritz Kiehn <msmk@cern.ch>

#ifndef ACTFW_IALGORITHM_H
#define ACTFW_IALGORITHM_H

#include <memory>
#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// Interface for event processing algorithms.
class IAlgorithm
{
public:
  /// Virtual Destructor
  virtual ~IAlgorithm() {}

  /// Framework name() method
  virtual std::string
  name() const = 0;

  /// Framework intialize method
  virtual ProcessCode
  initialize()
      = 0;

  /// Framework finalize mehtod
  virtual ProcessCode
  finalize()
      = 0;

  /// Framework execute method
  virtual ProcessCode
  execute(AlgorithmContext context) const = 0;
};

}  // namespace FW

#endif  // ACTFW_IALGORITHM_H
