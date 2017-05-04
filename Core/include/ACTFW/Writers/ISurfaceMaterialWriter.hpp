///////////////////////////////////////////////////////////////////
// ISurfaceMaterialWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_WRITERS_IMATERIALWRITER_H
#define ACTFW_WRITERS_IMATERIALWRITER_H

#include <memory>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
  class SurfaceMaterial;
  class GeometryID;
}

namespace FW {

/// @class ISurfaceMaterialWriter
///
/// Interface class for writing out the material
///

class ISurfaceMaterialWriter : public IService {
 public:
  /// Virtual destructor
  virtual ~ISurfaceMaterialWriter() = default;

  /// Writes out the material map of the layer
  virtual FW::ProcessCode write(
      const Acts::SurfaceMaterial& material,
      const Acts::GeometryID& geoID, 
      const std::string& name) = 0;
};

}
#endif  // ACTFW_WRITERS_IMATERIALWRITER_H
