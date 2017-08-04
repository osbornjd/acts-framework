#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

FWA::ReadEvgenAlgorithm::ReadEvgenAlgorithm(
    const Config&                       cfg,
    std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

std::string
FWA::ReadEvgenAlgorithm::name() const
{
  return "ReadEvgenAlgorithm";
}

FW::ProcessCode
FWA::ReadEvgenAlgorithm::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::ReadEvgenAlgorithm::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::ReadEvgenAlgorithm::skip(size_t nEvents)
{
  // there is a hard scatter evgen reader
  std::vector<Acts::ParticleProperties> skipParticles;
  if (m_cfg.hardscatterParticleReader
      && m_cfg.hardscatterParticleReader->read(skipParticles, nEvents)
          == FW::ProcessCode::ABORT) {
    // error and abort
    ACTS_ERROR("Could not skip " << nEvents << ". Aborting.");
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::ReadEvgenAlgorithm::read(FW::AlgorithmContext ctx)
{
  ACTS_DEBUG("Reading in genertated event info for event no. "
             << ctx.eventNumber);

  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);

  // Setup random number distributions for some quantities
  FW::PoissonDist pileupDist(m_cfg.pileupPoissonParameter);
  FW::GaussDist   vertexTDist(m_cfg.vertexTParameters[0],
                            m_cfg.vertexTParameters[1]);
  FW::GaussDist   vertexZDist(m_cfg.vertexZParameters[0],
                            m_cfg.vertexZParameters[1]);

  // prepare the output vector
  std::vector<Acts::ParticleProperties> eventParticles;

  // get the hard scatter if you have it
  std::vector<Acts::ParticleProperties> hardscatterParticles = {};
  if (m_cfg.hardscatterParticleReader
      && m_cfg.hardscatterParticleReader->read(hardscatterParticles)
          == FW::ProcessCode::ABORT) {
    ACTS_ERROR("Could not read hard scatter event. Aborting.");
    return FW::ProcessCode::ABORT;
  }
  ACTS_VERBOSE("- [HS X] number of hard scatter particles   : "
               << (hardscatterParticles.size() > 0 ? 1 : 0));

  // generate the number of pileup events
  size_t nPileUpEvents = m_cfg.randomNumbers ? size_t(pileupDist(rng)) : 0;

  ACTS_VERBOSE("- [PU X] number of in-time pileup events : " << nPileUpEvents);

  // reserve a lot
  eventParticles.reserve((nPileUpEvents)*hardscatterParticles.size() * 2);

  //
  // reserve quite a lot of space
  double vertexX = vertexTDist(rng);
  double vertexY = vertexTDist(rng);
  double vertexZ = vertexZDist(rng);

  Acts::Vector3D vertex(vertexX, vertexY, vertexZ);

  // fill in the particles
  barcode_type pCounter = 0;
  for (auto& hsParticle : hardscatterParticles) {
    // shift the particle by the vertex
    hsParticle.shift(vertex);
    hsParticle.assign(m_cfg.barcodeSvc->generate(0, pCounter++));
    // now push-back
    eventParticles.push_back(hsParticle);
  }

  // loop over the pile-up vertices
  for (size_t ipue = 0; ipue < nPileUpEvents; ++ipue) {
    // reserve quite a lot of space
    double         puVertexX = vertexTDist(rng);
    double         puVertexY = vertexTDist(rng);
    double         puVertexZ = vertexZDist(rng);
    Acts::Vector3D puVertex(puVertexX, puVertexY, puVertexZ);
    // get the vertices per pileup event
    std::vector<Acts::ParticleProperties> pileupPartiles = {};
    if (m_cfg.pileupParticleReader
        && m_cfg.pileupParticleReader->read(pileupPartiles)
            == FW::ProcessCode::ABORT) {
      ACTS_ERROR("Could not read pile up event " << ipue << ". Aborting.");
      return FW::ProcessCode::ABORT;
    }
    pCounter = 0;
    ACTS_VERBOSE("- [PU " << ipue << "] number of pile-up particles : "
                          << pileupPartiles.size()
                          << " - with z vertex position: " << puVertexZ);
    // loop over pileupParicles
    for (auto& puParticle : pileupPartiles) {
      // shift to the pile-up vertex
      puParticle.shift(puVertex);
      puParticle.assign(m_cfg.barcodeSvc->generate(ipue + 1, pCounter++));
      // now store the particle
      eventParticles.push_back(puParticle);
    }
  }

  // write to file if you have
  if (m_cfg.particleWriter
      && m_cfg.particleWriter->write(eventParticles)
          == FW::ProcessCode::ABORT) {
    ACTS_WARNING("Could not write colleciton of particles to file. Aborting.");
    return FW::ProcessCode::ABORT;
  }

  // write to the EventStore
  if (ctx.eventStore.add(m_cfg.evgenParticlesCollection,
                         std::move(eventParticles))
      == FW::ProcessCode::ABORT) {
    ACTS_WARNING(
        "Could not write colleciton of process vertices to event store.");
    return FW::ProcessCode::ABORT;
  }

  return FW::ProcessCode::SUCCESS;
}
