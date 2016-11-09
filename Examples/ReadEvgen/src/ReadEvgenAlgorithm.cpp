#include <iostream>
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

FWE::ReadEvgenAlgorithm::ReadEvgenAlgorithm(
      const Config&                 cfg,
      std::unique_ptr<Acts::Logger> logger)
  : m_cfg(cfg)
  , m_logger(std::move(logger))
{
}


FW::ProcessCode
FWE::ReadEvgenAlgorithm::skip(size_t nEvents)
{
  // there is a hard scatter evgen reader
  if (m_cfg.hardscatterEvgenReader)
      m_cfg.hardscatterEvgenReader->skip(nEvents);
  
  return FW::ProcessCode::SUCCESS;  
}

FW::ProcessCode
FWE::ReadEvgenAlgorithm::read(const FW::AlgorithmContext context) const
{
  
  // Retrieve relevant information from the execution context
  size_t eventNumber = context.eventContext->eventNumber;
  auto eventStore    = context.eventContext->eventStore;
  
  ACTS_DEBUG("Reading in genertated event info for event no. " << eventNumber);
  
  // Create a random number generator
  FW::RandomNumbersSvc::Generator rng =
    m_cfg.pileupRandomNumbers->spawnGenerator(context);
  
  // prepare the output vector
  std::vector< std::unique_ptr<Acts::ProcessVertex> >* eventVertices
    = new std::vector< std::unique_ptr<Acts::ProcessVertex> >;
  
  // get the hard scatter if you have it
  std::vector< Acts::ProcessVertex > hardscatterVertices = {};
  if (m_cfg.hardscatterEvgenReader)
      hardscatterVertices = m_cfg.hardscatterEvgenReader->processVertices();
  
  ACTS_VERBOSE("- number of hard scatter events   : "
                         << (hardscatterVertices.size()> 0 ? 1 : 0) );
  
  // generate the number of pileup events
  size_t nPileUpEvents = (m_cfg.pileupEvents && m_cfg.pileupRandomNumbers) ?
    size_t(rng.draw(FW::Distribution::poisson)) : 0;
  
  ACTS_VERBOSE("- number of in-time pileup events : " << nPileUpEvents);
  
  // pile-up events have one vertex usually
  eventVertices->reserve(nPileUpEvents+hardscatterVertices.size());
  
  // fill in the hard scatter vertics
  for (auto& hsVertex : hardscatterVertices)
    eventVertices->push_back(std::make_unique<Acts::ProcessVertex>(hsVertex));
  
  // loop over the pile-up vertices
  for (size_t ipue = 0; ipue < nPileUpEvents; ++ipue){
    // get the vertices per pileup event
    auto pileupVertices = m_cfg.pileupEvgenReader->processVertices();
    for (auto& puVertex : pileupVertices)
      eventVertices->push_back(std::make_unique<Acts::ProcessVertex>(puVertex));
  }
  
  // tell how many signal vertices to be processed
  ACTS_VERBOSE("- total number of signal vertices : " << eventVertices->size());
  
  // write to the EventStore
  if (eventStore
      && eventStore->writeT(eventVertices, m_cfg.vertexCollectionName)
      == FW::ProcessCode::ABORT){
    ACTS_WARNING("Could not write colleciton of process vertices to event store.");
    return FW::ProcessCode::ABORT;  
  }
  
  return FW::ProcessCode::SUCCESS;  
}

FW::ProcessCode
FWE::ReadEvgenAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  m_cfg.jBoard = jStore;
  return FW::ProcessCode::SUCCESS;
}


FW::ProcessCode
FWE::ReadEvgenAlgorithm::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

