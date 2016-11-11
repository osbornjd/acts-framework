//  IParticlePropertiesWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_WRITERS_IPARTICLEPROPERTIESWRITER_H
#define ACTFW_WRITERS_IPARTICLEPROPERTIESWRITER_H 1

#include "ACTS/EventData/ParticleProperties.hpp"

namespace FW {

/// @class IParticlePropertiesWriter
///
/// Interface class for extrapolation cell writers
///
class IParticlePropertiesWriter : public IService
{
public:
  /// Virtual Destructor
  virtual ~IParticlePropertiesWriter() {}

  /// The write interface
  virtual ProcessCode
  write(const std::vector<Acts::ParticleProperties>& pProteries) = 0;

};
}

#endif  // ACTFW_WRITERS_IPARTICLEPROPERTIESWRITER_H
