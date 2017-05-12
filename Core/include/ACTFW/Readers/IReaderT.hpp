///////////////////////////////////////////////////////////////////
// IReaderT.h
///////////////////////////////////////////////////////////////////
#ifndef ACTFW_READERS_IREADERT_H
#define ACTFW_READERS_IREADERT_H

#include <string>
#include <vector>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// @class IWriterT
///
/// Interface to readin an object
template <class T> class IReaderT : public IService
{
public:
  
  /// Virtual destructor
  virtual ~IReaderT() = default;

  /// the write method
  /// @param object is the thing to be read back in
  /// @return is a ProcessCode indicating return/failure
  virtual ProcessCode
  read(T& object, size_t skip = 0) = 0;
  
};

}
#endif  // ACTFW_READERS_IREADERT_H

