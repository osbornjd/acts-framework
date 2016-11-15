#include <iostream>
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

FWE::ReadEvgenAlgorithm::ReadEvgenAlgorithm(
    const Config&                 cfg,
    std::unique_ptr<Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

FW::ProcessCode
FWE::ReadEvgenAlgorithm::skip(size_t nEvents)
{
  // there is a hard scatter evgen reader
  if (m_cfg.hardscatterParticleReader)
    m_cfg.hardscatterParticleReader->skip(nEvents);

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::ReadEvgenAlgorithm::read(const FW::AlgorithmContext context) const
{

  // Retrieve relevant information from the execution context
  size_t eventNumber = context.eventContext->eventNumber;
  auto   eventStore  = context.eventContext->eventStore;

  ACTS_DEBUG("Reading in genertated event info for event no. " << eventNumber);

  // Create a random number generator
  FW::RandomNumbersSvc::Generator rngPileup
      = m_cfg.pileupRandomNumbers->spawnGenerator(context);

  FW::RandomNumbersSvc::Generator rngVertexT
      = m_cfg.pileupVertexDistT->spawnGenerator(context);

  FW::RandomNumbersSvc::Generator rngVertexZ
      = m_cfg.pileupVertexDistZ->spawnGenerator(context);

  // prepare the output vector
  std::vector<Acts::ParticleProperties>* eventParticles
      = new std::vector<Acts::ParticleProperties>;

  // get the hard scatter if you have it
  std::vector<Acts::ParticleProperties> hardscatterParticles = {};
  if (m_cfg.hardscatterParticleReader)
    hardscatterParticles = m_cfg.hardscatterParticleReader->particles();

  ACTS_VERBOSE("- [HS X] number of hard scatter particles   : "
               << (hardscatterParticles.size() > 0 ? 1 : 0));

  // generate the number of pileup events
  size_t nPileUpEvents = m_cfg.pileupRandomNumbers
      ? size_t(rngPileup.draw(FW::Distribution::poisson))
      : 0;

  ACTS_VERBOSE("- [PU X] number of in-time pileup events : " << nPileUpEvents);

  // reserve a lot
  eventParticles->reserve((nPileUpEvents)*hardscatterParticles.size() * 2);

  //
  // reserve quite a lot of space
  double vertexX = rngVertexT.draw(FW::Distribution::gauss);
  double vertexY = rngVertexT.draw(FW::Distribution::gauss);
  double vertexZ = rngVertexZ.draw(FW::Distribution::gauss);

  Acts::Vector3D vertex(vertexX, vertexY, vertexZ);

  // fill in the particles
  for (auto& hsParticle : hardscatterParticles) {
    // shift the particle by the vertex
    hsParticle.shift(vertex);
    // now push-back
    eventParticles->push_back(hsParticle);
  }

  // loop over the pile-up vertices
  for (size_t ipue = 0; ipue < nPileUpEvents; ++ipue) {
    // reserve quite a lot of space
    double         puVertexX = rngVertexT.draw(FW::Distribution::gauss);
    double         puVertexY = rngVertexT.draw(FW::Distribution::gauss);
    double         puVertexZ = rngVertexZ.draw(FW::Distribution::gauss);
    Acts::Vector3D puVertex(puVertexX, puVertexY, puVertexZ);
    // get the vertices per pileup event
    auto pileupPartiles = m_cfg.pileupParticleReader->particles();
    ACTS_VERBOSE("- [PU " << ipue << "] number of pile-up particles : "
                          << pileupPartiles.size()
                          << " - with z vertex position: "
                          << puVertexZ);
    // loop over pileupParicles
    for (auto& puParticle : pileupPartiles) {
      puParticle.shift(puVertex);
      eventParticles->push_back(puParticle);
    }
  }

  // write to file if you have
  if (m_cfg.particleWriter
      && m_cfg.particleWriter->write(*eventParticles)
          == FW::ProcessCode::ABORT) {
    ACTS_WARNING(
        "Could not write colleciton of process vertices to writer file.");
    return FW::ProcessCode::ABORT;
  }

  // write to the EventStore
  if (eventStore
      && eventStore->writeT(eventParticles, m_cfg.particleCollectionName)
          == FW::ProcessCode::ABORT) {
    ACTS_WARNING(
        "Could not write colleciton of process vertices to event store.");
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
