/// @file
/// @date 2016-05-23
/// @author Andreas Salburger
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_ISERVICE_H
#define ACTFW_ISERVICE_H

#include <string>

#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// Interface for common services.
class IService
{
public:
  /// Virutal Destructor
  virtual ~IService() {}

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
};

}  // namespace FW

#endif  // ACTFW_ISERVICE_H
