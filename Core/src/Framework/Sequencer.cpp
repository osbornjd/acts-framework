#include "ACTFW/Concurrency/parallel_for.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

#include "ACTFW/Framework/Sequencer.hpp"

FW::Sequencer::Sequencer(const Sequencer::Config&      cfg,
                         std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

FW::Sequencer::~Sequencer()
{
}

FW::ProcessCode
FW::Sequencer::initializeEventLoop()
{
  ACTS_INFO(
      "=================================================================");
  ACTS_INFO("Initializing the event loop for:");
  ACTS_INFO("      -> " << m_cfg.ioAlgorithms.size() << " IO Algorithms");
  ACTS_INFO("      -> " << m_cfg.eventAlgorithms.size() << " Event Algorithms");

  // initialize the services
  for (auto& isvc : m_cfg.services) {
    if (isvc->initialize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }

  // initialize the i/o algorithms
  for (auto& ioalg : m_cfg.ioAlgorithms) {
    if (ioalg->initialize(m_cfg.jobStore)
        != ProcessCode::SUCCESS)
      return ProcessCode::ABORT;
  }

  // initialize the event algorithms
  for (auto& alg : m_cfg.eventAlgorithms) {
    if (alg->initialize(m_cfg.jobStore)
        != ProcessCode::SUCCESS)
      return ProcessCode::ABORT;
  }
  // return with success
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::processEventLoop(size_t nEvents, size_t skipEvents)
{
  ACTS_INFO(
      "=================================================================");
  ACTS_INFO("Processing the event loop:");
  
  // Setup the job context
  auto jobContext = std::make_shared<const JobContext>(
    nEvents, m_cfg.jobStore );

  // skip the events if necessary
  if (skipEvents) {
    ACTS_INFO("==> SKIP   " << skipEvents << " events.");
    for (auto& ioalg : m_cfg.ioAlgorithms)
      if (ioalg->skip(skipEvents) != ProcessCode::SUCCESS)
        return ProcessCode::ABORT;
  }
  // execute the event loop
  ACTFW_PARALLEL_FOR(ievent, 0, nEvents,
    const size_t eventNumber = skipEvents + ievent;
    ACTS_INFO("==> EVENT " << eventNumber << " <== start. ");
    
    // Setup the event and algorithm context
    const auto eventStore = std::make_shared<WhiteBoard>(
      Acts::getDefaultLogger("EventStore#"+std::to_string(eventNumber),
                             m_cfg.eventStoreLogLevel));
    const auto eventContext = std::make_shared<const EventContext>(
      eventNumber, eventStore, jobContext );
    size_t ialg = 0;
    
    // a) then call read on all io algoirhtms
    for (const auto& ioalg : m_cfg.ioAlgorithms) {
      if (ioalg->read({ ialg++, eventContext }) != ProcessCode::SUCCESS)
        ACTFW_PARALLEL_FOR_ABORT(ievent);
    }
    // b) now call execute for all event algorithms
    for (const auto& alg : m_cfg.eventAlgorithms) {
      if (alg->execute({ ialg++, eventContext }) != ProcessCode::SUCCESS)
        ACTFW_PARALLEL_FOR_ABORT(ievent);
    }
    // c) now call write to all io algoirhtms
    for (const auto& ioalg : m_cfg.ioAlgorithms) {
      if (ioalg->write({ ialg++, eventContext }) != ProcessCode::SUCCESS)
        ACTFW_PARALLEL_FOR_ABORT(ievent);
    }
    ACTS_INFO("<== EVENT " << eventNumber << " ==> done. ");
  )
  // return with success
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Sequencer::finalizeEventLoop()
{
  ACTS_INFO(
      "=================================================================");
  ACTS_INFO("Finalize the event loop for:");
  ACTS_INFO("      -> " << m_cfg.ioAlgorithms.size() << " IO Algorithms");
  ACTS_INFO("      -> " << m_cfg.eventAlgorithms.size() << " Event Algorithms");

  // finalize the services
  for (auto& isvc : m_cfg.services) {
    if (isvc->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }

  // finalize the i/o algorithms
  for (auto& ioalg : m_cfg.ioAlgorithms) {
    if (ioalg->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }

  // finalize the event algorithms
  for (auto& alg : m_cfg.eventAlgorithms) {
    if (alg->finalize() != ProcessCode::SUCCESS) return ProcessCode::ABORT;
  }
  // return with success
  return ProcessCode::SUCCESS;
}
