///////////////////////////////////////////////////////////////////
// IMaterialStepWriter.h
///////////////////////////////////////////////////////////////////

#ifndef WRITERS_IMATERIALSTEPWRITER_H
#define WRITERS_IMATERIALSTEPWRITER_H

#include <string>
#include <vector>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Definitions.hpp"

namespace Acts {
class MaterialStep;
class Surface;
}
namespace FW {

/// @class IMaterialStepWriter
/// Interface to write out a vector of MaterialStep entities per surface
///

class IMaterialStepWriter : public IService
{
public:
  /// Virtual destructor
  virtual ~IMaterialStepWriter() = default;

  /// Writes out the MaterialStep entities
  /// @param surface the underlying Acts::Surface corresponding to the material
  /// steps and layer positions
  /// @param stepAndLayerPos a pair of the total Acts::MaterialSteps for this
  /// Surface and the respective assigned position on the layer
  virtual ProcessCode
  write(std::string          name,
        const Acts::Surface* surface,
        const std::vector<std::pair<const Acts::MaterialStep,
                                    const Acts::Vector3D>> stepsAndLayerPos)
      = 0;
};
}
#endif  // WRITERS_IMATERIALSTEPWRITER_H
