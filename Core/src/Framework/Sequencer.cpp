#include "ACTFW/Framework/Sequencer.hpp"

#include <algorithm>

#include "ACTFW/Concurrency/parallel_for.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

FW::Sequencer::Sequencer(const Sequencer::Config&            cfg,
                         std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

FW::Sequencer::~Sequencer()
{
}

FW::ProcessCode
FW::Sequencer::addServices(std::vector<std::shared_ptr<FW::IService>> services)
{
  for (auto& svc : services) {
    if (!svc) {
      ACTS_FATAL("Trying to add empty service to sequencer");
      return ProcessCode::ABORT;
    }
    m_services.push_back(std::move(svc));
    ACTS_INFO("Added service " << m_services.back()->name());
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::addReaders(std::vector<std::shared_ptr<FW::IReader>> readers)
{
  for (auto& rdr : readers) {
    if (!rdr) {
      ACTS_FATAL("Trying to add empty reader to sequencer");
      return ProcessCode::ABORT;
    }
    m_readers.push_back(std::move(rdr));
    ACTS_INFO("Added reader " << m_readers.back()->name());
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::addWriters(std::vector<std::shared_ptr<FW::IWriter>> writers)
{
  for (auto& wrt : writers) {
    if (!wrt) {
      ACTS_FATAL("Trying to add empty writer to sequencer");
      return ProcessCode::ABORT;
    }
    m_writers.push_back(std::move(wrt));
    ACTS_INFO("Added writer " << m_writers.back()->name());
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::prependEventAlgorithms(
    std::vector<std::shared_ptr<FW::IAlgorithm>> algorithms)
{
  for (auto& alg : algorithms) {
    if (!alg) {
      ACTS_FATAL("Trying to prepend empty algorithm");
      return ProcessCode::ABORT;
    }
    m_algorithms.insert(m_algorithms.begin(), std::move(alg));
    ACTS_INFO("Prepended algorithm " << m_algorithms.front()->name());
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::appendEventAlgorithms(
    std::vector<std::shared_ptr<FW::IAlgorithm>> algorithms)
{
  for (auto& alg : algorithms) {
    if (!alg) {
      ACTS_FATAL("Trying to append empty algorithm.");
      return ProcessCode::ABORT;
    }
    m_algorithms.push_back(std::move(alg));
    ACTS_INFO("Appended algorithm " << m_algorithms.back()->name());
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::run(size_t events, size_t skip)
{
  // initialize services and algorithms
  ACTS_INFO("Initialize the event loop for");
  ACTS_INFO("  " << m_services.size() << " services");
  ACTS_INFO("  " << m_readers.size() << " readers");
  ACTS_INFO("  " << m_writers.size() << " writers");
  ACTS_INFO("  " << m_algorithms.size() << " algorithms");
  for (auto& svc : m_services)
    if (svc->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;

  // execute the event loop
  ACTS_INFO("Run the event loop");
  ACTFW_PARALLEL_FOR(
      ievent, 0, events, const size_t event = skip + ievent;
      ACTS_INFO("start event " << event);

      // Setup the event and algorithm context
      WhiteBoard eventStore(Acts::getDefaultLogger(
          "EventStore#" + std::to_string(event), m_cfg.eventStoreLogLevel));
      size_t     ialg = 0;

      // read everything in
      for (auto& rdr
           : m_readers) {
        if (rdr->read({ialg++, event, eventStore}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }
      // process all algorithms
      for (auto& alg
           : m_algorithms) {
        if (alg->execute({ialg++, event, eventStore}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }
      // write out results
      for (auto& wrt
           : m_writers) {
        if (wrt->write({ialg++, event, eventStore}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }

      ACTS_INFO("event " << event << " done");)

  // finalize algorithms and services in reverse order
  ACTS_INFO("Finalize the event loop for");
  ACTS_INFO("  " << m_services.size() << " services");
  ACTS_INFO("  " << m_readers.size() << " readers");
  ACTS_INFO("  " << m_writers.size() << " writers");
  ACTS_INFO("  " << m_algorithms.size() << " algorithms");
  for (auto& wrt : m_writers)
    if (wrt->endRun() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& svc : m_services) {
    if (svc->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
    if (svc->endRun() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }
  return ProcessCode::SUCCESS;
}
