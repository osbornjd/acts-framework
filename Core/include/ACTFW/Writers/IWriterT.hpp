///////////////////////////////////////////////////////////////////
// IWriterT.h
///////////////////////////////////////////////////////////////////
#ifndef ACTFW_WRITERS_IWRITERT_H
#define ACTFW_WRITERS_IWRITERT_H

#include <string>
#include <vector>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
  class Surface;
}

namespace FW {

/// @class IWriterT
///
/// Interface to write out the single objects
///
/// @note Use of this interface is deprecated, all event data writers should be
///       turned into descendents of the WriterT class in the long run
///
template <class T> class IWriterT : public IService
{
public:
  /// Virtual destructor
  virtual ~IWriterT() = default;

  /// the write method
  /// @param object is the object to be written out
  /// @return is a ProcessCode indicating return/failure
  virtual ProcessCode
  write(const T& object) = 0;
};

}
#endif  // ACTFW_WRITERS_IWRITERT_H