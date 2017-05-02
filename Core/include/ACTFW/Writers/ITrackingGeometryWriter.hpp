///////////////////////////////////////////////////////////////////
// ITrackingGeometryWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_WRITERS_ITRACKINGGEOMETRYWRITER_H
#define ACTFW_WRITERS_ITRACKINGGEOMETRYWRITER_H 1

#include <string>
#include <vector>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
  class TrackingGeometry;
}

namespace FW {

/// @class ITrackingGeometryWriter
///
/// Interface to write out the TrackingGeometry

class ITrackingGeometryWriter : public IService
{
public:
  
  /// Virtual destructor
  virtual ~ITrackingGeometryWriter() = default;

  /// the write method
  virtual ProcessCode
  write(const Acts::TrackingGeometry& tGeometry) = 0;
  
};

}
#endif  // ACTFW_WRITERS_ITRACKINGGEOMETRYWRITER_H