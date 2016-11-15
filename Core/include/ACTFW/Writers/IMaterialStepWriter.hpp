///////////////////////////////////////////////////////////////////
// IMaterialStepWriter.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALSTEPWRITER_H
#define WRITERS_IMATERIALSTEPWRITER_H

#include <string>
#include <vector>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
class MaterialStep;
class Surface;
}
namespace FW {

/// @class IMaterialStepWriter
/// Interface to write out a vector of MaterialStep entities per surface
///

class IMaterialStepWriter : public IService {
 public:
  /// Virtual destructor
  virtual ~IMaterialStepWriter() = default;

  /// Writes out the MaterialStep entities
  virtual ProcessCode write(std::string name, const Acts::Surface* surface,
                            std::vector<Acts::MaterialStep> msteps) = 0;
};
}
#endif  // WRITERS_IMATERIALSTEPWRITER_H
