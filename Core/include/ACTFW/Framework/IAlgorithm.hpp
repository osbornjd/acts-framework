//  IAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef IAlgorithm_h
#define IAlgorithm_h

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
  virtual const std::string&
  name() const = 0;

  /// Framework intialize method
  virtual ProcessCode
  initialize()
      = 0;

  /// Framework execute method
  virtual ProcessCode
  execute(const AlgorithmContext context) const = 0;

  /// Framework finalize mehtod
  virtual ProcessCode
  finalize()
      = 0;
};

}  // namespace FW

#endif  // IAlgorithm_h
