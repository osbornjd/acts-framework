///////////////////////////////////////////////////////////////////
// ISurfaceWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ACTFW_WRITERS_ISURFACEWRITER_H
#define ACTFW_WRITERS_ISURFACEWRITER_H 1

#include <string>
#include <vector>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
  class Surface;
}

namespace FW {

/// @class ISurfaceWriter
///
/// Interface to write out the Surfaces

class ISurfaceWriter : public IService
{
public:
  
  /// Virtual destructor
  virtual ~ISurfaceWriter() = default;

  /// the write method
  virtual ProcessCode
  write(const Acts::Surface& surface) = 0;
  
};

}
#endif  // ACTFW_WRITERS_ISURFACEWRITER_H