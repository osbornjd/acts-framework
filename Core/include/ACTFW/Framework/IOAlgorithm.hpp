//
//  IOAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_FRAMEWORK_IOALGORITHM_H
#define ACTFW_FRAMEWORK_IOALGORITHM_H 1

#include <memory>
#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

class WhiteBoard;

/// @class IOAlgorithm
///
/// Base class that defines the interface for algorithms that deal with I/O
class IOAlgorithm
{
public:
  /// Virtual destructor
  virtual ~IOAlgorithm() {}
  /// Framework intialize method
  virtual ProcessCode
  initialize(std::shared_ptr<WhiteBoard> jobStore   = nullptr)
      = 0;

  /// Skip a few events in the IO stream
  virtual ProcessCode
  skip(size_t nEvents = 1)
      = 0;

  /// Read out data from the input stream
  virtual ProcessCode
  read(const AlgorithmContext context) const
      = 0;

  /// Write data to the output stream
  virtual ProcessCode
  write(const AlgorithmContext context) const
      = 0;

  /// Framework finalize mehtod
  virtual ProcessCode
  finalize()
      = 0;

  /// Framework name() method
  virtual const std::string&
  name() const = 0;

  /// return the jobStore - things that live for the full job
  virtual std::shared_ptr<WhiteBoard>
  jobStore() const = 0;
};
}

#endif  // ACTFW_FRAMEWORK_IOALGORITHM_H
