#include "ACTFW/Fatras/FatrasWriteAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"

FWE::FatrasWriteAlgorithm::FatrasWriteAlgorithm(
    const Config&                 cfg,
    std::unique_ptr<Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

FW::ProcessCode
FWE::FatrasWriteAlgorithm::skip(size_t nEvents)
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::FatrasWriteAlgorithm::write(const FW::AlgorithmContext context) const
{

  // Retrieve relevant information from the execution context
  size_t eventNumber = context.eventContext->eventNumber;
  auto   eventStore  = context.eventContext->eventStore;

  // write the simulated Particles
  if (m_cfg.simulatedParticlesCollection != "" && m_cfg.particleWriter){
    // retrieve it
    std::vector<Acts::ParticleProperties>* simulatedParticles = nullptr;
    // read and go
    if (eventStore
        && eventStore->readT(simulatedParticles, m_cfg.simulatedParticlesCollection)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // run over it - and write them
    // write to file if you have
    if (m_cfg.particleWriter->write(*simulatedParticles) == FW::ProcessCode::ABORT) {
    ACTS_WARNING(
            "Could not write colleciton of simulated particles to writer.");
        return FW::ProcessCode::ABORT;
    }
  }
  
  // write the hits
  if (m_cfg.planarClustersCollection != "" && m_cfg.planarClusterWriter){
    // retrieve the input data
    FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>* planarClusters = nullptr;
    // read and go
    if (eventStore
        && eventStore->readT(planarClusters, m_cfg.planarClustersCollection)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // run over it - and write them
    if (m_cfg.planarClusterWriter->write(*planarClusters) == FW::ProcessCode::ABORT){
      ACTS_WARNING(
              "Could not colleciton of clusters to writer.");
          return FW::ProcessCode::ABORT;
    }
  }

  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::FatrasWriteAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  m_cfg.jBoard = jStore;
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::FatrasWriteAlgorithm::finalize()
{
  return FW::ProcessCode::SUCCESS;
}
