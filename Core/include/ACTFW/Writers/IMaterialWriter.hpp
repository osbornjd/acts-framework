///////////////////////////////////////////////////////////////////
// IMaterialWriter.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALWRITER_H
#define WRITERS_IMATERIALWRITER_H

#include <memory>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
class BinnedSurfaceMaterial;
class GeometryID;
}

namespace FW {

/// @class IMaterialWriter
///
/// Interface class for writing out the material
///

class IMaterialWriter : public IService {
 public:
  /// Virtual destructor
  virtual ~IMaterialWriter() = default;

  /// Writes out the material map of the layer
  virtual ProcessCode write(
      std::shared_ptr<const Acts::BinnedSurfaceMaterial> material,
      const Acts::GeometryID geoID, std::string name) = 0;
};
}
#endif  // WRITERS_IMATERIALWRITER_H
