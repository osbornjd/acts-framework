// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <Acts/Utilities/Logger.hpp>

#include "ACTFW/Framework/IAlgorithm.hpp"
#include "ACTFW/Framework/IContextDecorator.hpp"
#include "ACTFW/Framework/IReader.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/IWriter.hpp"

namespace FW {

/// @class  Sequencer
///
/// This is the backbone of the mini framework, it initializes all algorithms,
/// calls execute per event and deals with the event store.
///
class Sequencer
{
public:
  struct Config
  {
    /// job store logging level
    Acts::Logging::Level jobStoreLogLevel = Acts::Logging::INFO;
    /// event store logging level
    Acts::Logging::Level eventStoreLogLevel = Acts::Logging::INFO;
    /// number of threads to run in parallel, negative for automatic value
    int numThreads = -1;
  };

  /// Constructor
  ///
  /// @param cfg is the configuration object
  Sequencer(const Config&                       cfg,
            std::unique_ptr<const Acts::Logger> logger
            = Acts::getDefaultLogger("Sequencer", Acts::Logging::INFO));

  /// Add a service to the set of services.
  ///
  /// @throws std::invalid_argument if the service is NULL.
  void
  addService(std::shared_ptr<IService> service);
  /// Add a context decorator to the set of context decorators.
  ///
  /// @throws std::invalid_argument if the decorator is NULL.
  void
  addContextDecorator(std::shared_ptr<IContextDecorator> decorator);
  /// Add a reader to the set of readers.
  ///
  /// @throws std::invalid_argument if the reader is NULL.
  void
  addReader(std::shared_ptr<IReader> reader);
  /// Append an algorithm to the sequence of algorithms.
  ///
  /// @throws std::invalid_argument if the algorithm is NULL.
  void
  addAlgorithm(std::shared_ptr<IAlgorithm> algorithm);
  /// Add a writer to the set of writers.
  ///
  /// @throws std::invalid_argument if the writer is NULL.
  void
  addWriter(std::shared_ptr<IWriter> writer);

  /// Run the event loop over the given number of events.
  ///
  /// @param events Number of events to process
  /// @param skip Number of events to skip before processing
  /// @return status code compatible with the `main()` return code
  /// @returns EXIT_SUCCESS when everying worked without problems
  /// @returns EXIT_FAILURE if something went wrong
  ///
  /// @note If the number of events to process are not given, the sequencer
  /// will process events until the first reader signals the end-of-file. If
  /// given, it sets an upper bound.
  ///
  /// This function is intended to be run as the last thing in the tool
  /// main function and its return value can be used directly as the program
  /// return value, i.e.
  ///
  ///     int main(int argc, char* argv[])
  ///     {
  ///         Sequencer seq;
  ///         ... // set up the algorithms
  ///         return seq.run(...);
  ///     }
  ///
  /// This will run the start-of-run hook for all configured services, run all
  /// configured readers, algorithms, and writers for each event, then invoke
  /// the enf-of-run hook of writers and services.
  int
  run(std::optional<size_t> events, size_t skip = 0);

private:
  std::vector<std::shared_ptr<IService>>          m_services;
  std::vector<std::shared_ptr<IContextDecorator>> m_decorators;
  std::vector<std::shared_ptr<IReader>>           m_readers;
  std::vector<std::shared_ptr<IAlgorithm>>        m_algorithms;
  std::vector<std::shared_ptr<IWriter>>           m_writers;
  Config                                          m_cfg;
  std::unique_ptr<const Acts::Logger>             m_logger;

  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW
