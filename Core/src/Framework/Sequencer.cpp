// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Framework/Sequencer.hpp"

#include <algorithm>
#include <cstdlib>
#include <exception>

#include <TROOT.h>
#include <tbb/tbb.h>

#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

FW::Sequencer::Sequencer(const Sequencer::Config&            cfg,
                         std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
  // automatically determine the number of concurrent threads to use
  if (m_cfg.numThreads < 0) {
    const char* numThreadsEnv = getenv("ACTSFW_NUM_THREADS");
    if (numThreadsEnv) {
      m_cfg.numThreads = std::stoi(numThreadsEnv);
    } else {
      m_cfg.numThreads = tbb::task_scheduler_init::default_num_threads();
    }
  }
  ROOT::EnableThreadSafety();
}

void
FW::Sequencer::addService(std::shared_ptr<IService> service)
{
  if (not service) {
    throw std::invalid_argument("Can not add empty/NULL service");
  }
  m_services.push_back(std::move(service));
  ACTS_INFO("Added service '" << m_services.back()->name() << "'");
}

void
FW::Sequencer::addContextDecorator(std::shared_ptr<IContextDecorator> decorator)
{
  if (not decorator) {
    throw std::invalid_argument("Can not add empty/NULL context decorator");
  }
  m_decorators.push_back(std::move(decorator));
  ACTS_INFO("Added context decarator '" << m_decorators.back()->name() << "'");
}

void
FW::Sequencer::addReader(std::shared_ptr<IReader> reader)
{
  if (not reader) {
    throw std::invalid_argument("Can not add empty/NULL reader");
  }
  m_readers.push_back(std::move(reader));
  ACTS_INFO("Added reader '" << m_readers.back()->name() << "'");
}

void
FW::Sequencer::addAlgorithm(std::shared_ptr<IAlgorithm> algorithm)
{
  if (not algorithm) {
    throw std::invalid_argument("Can not add empty/NULL algorithm");
  }
  m_algorithms.push_back(std::move(algorithm));
  ACTS_INFO("Added algorithm '" << m_algorithms.back()->name() << "'");
}

void
FW::Sequencer::addWriter(std::shared_ptr<IWriter> writer)
{
  if (not writer) {
    throw std::invalid_argument("Can not add empty/NULL writer");
  }
  m_writers.push_back(std::move(writer));
  ACTS_INFO("Added writer '" << m_writers.back()->name() << "'");
}

std::vector<std::string>
FW::Sequencer::listAlgorithmNames() const
{
  std::vector<std::string> names;

  // WARNING this must be done in the same order as in the processing
  for (const auto& decorator : m_decorators) {
    names.push_back("decorator:" + decorator->name());
  }
  for (const auto& reader : m_readers) {
    names.push_back("reader:" + reader->name());
  }
  for (const auto& algorithm : m_algorithms) {
    names.push_back("algorithm:" + algorithm->name());
  }
  for (const auto& writer : m_writers) {
    names.push_back("writer:" + writer->name());
  }

  return names;
}

std::optional<size_t>
FW::Sequencer::determineEndEvent(std::optional<size_t> requested,
                                 size_t                skip) const
{
  // There are two possibilities how the event loop can be steered
  // 1) By the number of requested events
  // 2) By the number of events available in the readers

  auto shortestReader = std::min_element(
      m_readers.begin(), m_readers.end(), [](const auto& a, const auto& b) {
        return (a->numEvents() < b->numEvents());
      });

  if (shortestReader != m_readers.end()) {
    // at least one reader is available and there is a maximum number of events
    size_t endOnFile = (*shortestReader)->numEvents();
    if (endOnFile <= skip) {
      // trying to skip too many events is an error
      ACTS_ERROR("Number of skipped events > than available number of events");
      return std::optional<size_t>();
    } else if (not requested) {
      // without explicit limit, process all events on file after skipping
      return std::make_optional(endOnFile);
    } else {
      // with explicit limit, take the smallest value
      size_t endRequested = skip + requested.value();
      if (endOnFile < endRequested) {
        ACTS_INFO("Restrict number of events to available events");
        return std::make_optional(endOnFile);
      } else {
        return std::make_optional(endRequested);
      }
    }
  } else {
    // no readers configure, number of events must be manually specified
    if (skip != 0) {
      // without readers, skipping has no meaning
      ACTS_ERROR("Can not skip events without configured readers");
      return std::optional<size_t>();
    } else if (not requested) {
      ACTS_ERROR("Missing number of events without configured readers");
      return std::optional<size_t>();
    } else {
      return std::make_optional(requested.value());
    }
  }
}

int
FW::Sequencer::run(std::optional<size_t> events, size_t skip)
{
  // processing only works w/ a well-known number of events
  // error message are handled by helper function
  auto endEvent = determineEndEvent(events, skip);
  if (not endEvent) { return EXIT_FAILURE; }

  ACTS_INFO("Starting event loop with " << m_cfg.numThreads << " threads");
  ACTS_INFO("  " << m_services.size() << " services");
  ACTS_INFO("  " << m_decorators.size() << " context decorators");
  ACTS_INFO("  " << m_readers.size() << " readers");
  ACTS_INFO("  " << m_algorithms.size() << " algorithms");
  ACTS_INFO("  " << m_writers.size() << " writers");

  std::vector<std::string> names = listAlgorithmNames();

  // Execute the event loop
  tbb::task_scheduler_init init(m_cfg.numThreads);
  tbb::parallel_for(
      tbb::blocked_range<size_t>(skip, endEvent.value() + 1),
      [&](const tbb::blocked_range<size_t>& r) {
        for (size_t event = r.begin(); event != r.end(); ++event) {
          // Use per-event store
          WhiteBoard eventStore(Acts::getDefaultLogger(
              "EventStore#" + std::to_string(event), m_cfg.eventStoreLogLevel));
          // If we ever wanted to run algorithms in parallel, this needs to be
          // changed to Algorithm context copies
          AlgorithmContext context(0, event, eventStore);

          /// Decorate the context
          for (auto& cdr : m_decorators) {
            if (cdr->decorate(++context) != ProcessCode::SUCCESS) {
              throw std::runtime_error("Failed to decorate event context");
            }
          }
          // Read everything in
          for (auto& rdr : m_readers) {
            if (rdr->read(++context) != ProcessCode::SUCCESS) {
              throw std::runtime_error("Failed to read input data");
            }
          }
          // Process all algorithms
          for (auto& alg : m_algorithms) {
            if (alg->execute(++context) != ProcessCode::SUCCESS) {
              throw std::runtime_error("Failed to process event data");
            }
          }
          // Write out results
          for (auto& wrt : m_writers) {
            if (wrt->write(++context) != ProcessCode::SUCCESS) {
              throw std::runtime_error("Failed to write output data");
            }
          }
          ACTS_INFO("finished event " << event);
        }
      });

  // Call endRun() for writers and services
  for (auto& wrt : m_writers) {
    if (wrt->endRun() != ProcessCode::SUCCESS) { return EXIT_FAILURE; }
  }
  for (auto& svc : m_services) {
    if (svc->endRun() != ProcessCode::SUCCESS) { return EXIT_FAILURE; }
  }
  return EXIT_SUCCESS;
}
