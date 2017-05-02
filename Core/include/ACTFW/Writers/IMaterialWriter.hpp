///////////////////////////////////////////////////////////////////
// IMaterialWriter.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALWRITER_H
#define WRITERS_IMATERIALWRITER_H

#include <memory>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
  class SurfaceMaterial;
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
  virtual FW::ProcessCode write(
      const Acts::SurfaceMaterial& material,
      const Acts::GeometryID& geoID, 
      const std::string& name) = 0;
};

}
#endif  // WRITERS_IMATERIALWRITER_H
