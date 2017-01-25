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
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/Utilities/GeometryID.hpp"

namespace FW {

/// @class IPlanarClusterWriter
///
/// Interface class for planar cluster writing
///
class IPlanarClusterWriter : public IService
{
public:
  /// Virtual Destructor
  virtual ~IPlanarClusterWriter() {}

  /// The write interface
  virtual ProcessCode
  write(const DetectorData<geo_id_value, Acts::PlanarModuleCluster>& ) = 0;

};

}

#endif  // ACTFW_WRITERS_IPLANARCLUSTERWRITER_H
