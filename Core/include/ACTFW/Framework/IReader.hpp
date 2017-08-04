/// @file
/// @date 2017-07-25
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_IREADER_H
#define ACTFW_IREADER_H

#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// Interface for reading data.
class IReader
{
public:
  virtual ~IReader() {}

  /// Provide the name of the reader
  virtual std::string
  name() const = 0;

  /// Initialize the reader, e.g. open files.
  virtual ProcessCode
  initialize()
      = 0;

  /// Finalize the reader, e.g. close files.
  virtual ProcessCode
  finalize()
      = 0;

  /// Skip the next n events.
  virtual ProcessCode
  skip(size_t skip)
      = 0;

  /// Read the next event
  virtual ProcessCode
  read(AlgorithmContext context)
      = 0;
};

}  // namespace FW

#endif  // ACTFW_IREADER_H
