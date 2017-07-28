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
    m_cfg.services.push_back(std::move(svc));
    ACTS_INFO("Added service " << m_cfg.services.back()->name());
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
    m_cfg.readers.push_back(std::move(rdr));
    ACTS_INFO("Added reader " << m_cfg.readers.back()->name());
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
    m_cfg.writers.push_back(std::move(wrt));
    ACTS_INFO("Added writer " << m_cfg.writers.back()->name());
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
    m_cfg.algorithms.insert(m_cfg.algorithms.begin(), std::move(alg));
    ACTS_INFO("Prepended algorithm " << m_cfg.algorithms.front()->name());
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
    m_cfg.algorithms.push_back(std::move(alg));
    ACTS_INFO("Appended algorithm " << m_cfg.algorithms.back()->name());
  }
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::run(size_t events, size_t skip)
{
  // automatically create a private job store
  auto jobStore = std::make_shared<WhiteBoard>(
      Acts::getDefaultLogger("JobStore", m_cfg.jobStoreLogLevel));
  auto jobCtx = std::make_shared<JobContext>(events, jobStore);

  // initialize services and algorithms
  ACTS_INFO("Initialize the event loop for");
  ACTS_INFO("  -> " << m_cfg.services.size() << " services");
  ACTS_INFO("  -> " << m_cfg.readers.size() << " readers");
  ACTS_INFO("  -> " << m_cfg.writers.size() << " writers");
  ACTS_INFO("  -> " << m_cfg.algorithms.size() << " algorithms");
  for (auto& svc : m_cfg.services)
    if (svc->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& rdr : m_cfg.readers)
    if (rdr->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& wrt : m_cfg.writers)
    if (wrt->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& alg : m_cfg.algorithms)
    if (alg->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;

  // execute the event loop
  ACTS_INFO("Run the event loop");
  ACTFW_PARALLEL_FOR(
      ievent, 0, events, const size_t event = skip + ievent;
      ACTS_INFO("start event " << event);

      // Setup the event and algorithm context
      auto eventStore = std::make_shared<WhiteBoard>(Acts::getDefaultLogger(
          "EventStore#" + std::to_string(event), m_cfg.eventStoreLogLevel));
      auto eventCtx
      = std::make_shared<const EventContext>(event, eventStore, jobCtx);
      size_t ialg = 0;

      // read everything in
      for (auto& rdr
           : m_cfg.readers) {
        if (rdr->read({ialg++, eventCtx}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }
      // process all algorithms
      for (auto& alg
           : m_cfg.algorithms) {
        if (alg->execute({ialg++, eventCtx}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }
      // write out results
      for (auto& wrt
           : m_cfg.writers) {
        if (wrt->write({ialg++, eventCtx}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }

      ACTS_INFO("event " << event << " done");)

  // finalize algorithms and services in reverse order
  ACTS_INFO("Finalize the event loop for");
  ACTS_INFO("  -> " << m_cfg.services.size() << " services");
  ACTS_INFO("  -> " << m_cfg.readers.size() << " readers");
  ACTS_INFO("  -> " << m_cfg.writers.size() << " writers");
  ACTS_INFO("  -> " << m_cfg.algorithms.size() << " algorithms");
  for (auto& alg : m_cfg.algorithms)
    if (alg->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& wrt : m_cfg.writers)
    if (wrt->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& rdr : m_cfg.readers)
    if (rdr->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  for (auto& svc : m_cfg.services)
    if (svc->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
}
