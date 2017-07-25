#include "ACTFW/Framework/Sequencer.hpp"

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
FW::Sequencer::run(size_t events, size_t skip)
{
  // automatically create a private job store
  auto jobStore = std::make_shared<WhiteBoard>(
      Acts::getDefaultLogger("JobStore", m_cfg.jobStoreLogLevel));
  auto jobCtx = std::make_shared<JobContext>(events, jobStore);

  // initialize services and algorithms
  ACTS_INFO("Initialize the event loop for");
  ACTS_INFO("  -> " << m_cfg.services.size() << " Services");
  ACTS_INFO("  -> " << m_cfg.ioAlgorithms.size() << " IO Algorithms");
  ACTS_INFO("  -> " << m_cfg.eventAlgorithms.size() << " Event Algorithms");
  for (auto& svc : m_cfg.services) {
    if (svc->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }
  for (auto& ioalg : m_cfg.ioAlgorithms) {
    if (ioalg->initialize(jobStore) != ProcessCode::SUCCESS)
      return ProcessCode::ABORT;
  }
  for (auto& alg : m_cfg.eventAlgorithms) {
    if (alg->initialize(jobStore) != ProcessCode::SUCCESS)
      return ProcessCode::ABORT;
  }

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

      // a) then call read on all io algoirhtms
      for (const auto& ioalg
           : m_cfg.ioAlgorithms) {
        if (ioalg->read({ialg++, eventCtx}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }
      // b) now call execute for all event algorithms
      for (const auto& alg
           : m_cfg.eventAlgorithms) {
        if (alg->execute({ialg++, eventCtx}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      }
      // c) now call write to all io algoirhtms
      for (const auto& ioalg
           : m_cfg.ioAlgorithms) {
        if (ioalg->write({ialg++, eventCtx}) != ProcessCode::SUCCESS)
          ACTFW_PARALLEL_FOR_ABORT(ievent);
      } ACTS_INFO("<== EVENT " << event << " ==> done. ");)

  // finalize algorithms and services in reverse order
  ACTS_INFO("Finalize the event loop for");
  ACTS_INFO("  -> " << m_cfg.services.size() << " Services");
  ACTS_INFO("  -> " << m_cfg.ioAlgorithms.size() << " IO Algorithms");
  ACTS_INFO("  -> " << m_cfg.eventAlgorithms.size() << " Event Algorithms");
  for (auto& alg : m_cfg.eventAlgorithms) {
    if (alg->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }
  for (auto& ioalg : m_cfg.ioAlgorithms) {
    if (ioalg->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }
  for (auto& svc : m_cfg.services) {
    if (svc->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }
}
