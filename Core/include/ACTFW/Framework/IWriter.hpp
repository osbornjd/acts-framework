/// @file
/// @date 2017-07-25
/// @author Moritz Kiehnn <msmk@cern.ch>

#ifndef ACTFW_IWRITER_H
#define ACTFW_IWRITER_H

#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// Interface for writing data.
class IWriter
{
public:
  virtual ~IWriter() {}

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

  /// write data to the output stream
  virtual ProcessCode
  write(const AlgorithmContext& context)
      = 0;

};

}  // namespace FW

#endif  // ACTFW_IWRITER_H
