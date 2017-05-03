//  IPlanarClusterWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_WRITERS_IPLANARCLUSTERWRITER_H
#define ACTFW_WRITERS_IPLANARCLUSTERWRITER_H 1

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTS/Utilities/GeometryID.hpp"

namespace FW {

/// @class IEventDataWriter
///
/// Interface class for writing EventData that are ordered
/// in DataContaineras
///
template <class T> class IEventDataWriter : public IService
{
public:
  /// Virtual Destructor
  virtual ~IEventDataWriter() {}

  /// The write interface
  virtual ProcessCode
  write(const DetectorData<geo_id_value, T>& ) = 0;

};

}

#endif  // ACTFW_WRITERS_IPLANARCLUSTERWRITER_H
