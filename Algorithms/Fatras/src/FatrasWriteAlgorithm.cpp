#include "ACTFW/Fatras/FatrasWriteAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Digitization/PlanarModuleCluster.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"

FWA::FatrasWriteAlgorithm::FatrasWriteAlgorithm(
    const Config&                       cfg,
    std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

std::string
FWA::FatrasWriteAlgorithm::name() const
{
  return "FatrasWriteAlgorithm";
}

FW::ProcessCode
FWA::FatrasWriteAlgorithm::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::FatrasWriteAlgorithm::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::FatrasWriteAlgorithm::write(const FW::AlgorithmContext& context)
{
  // Retrieve relevant information from the execution context
  size_t eventNumber = context.eventNumber;
  auto   eventStore  = context.eventStore;

  ACTS_INFO("Writing Fatras output to file(s).");

  // write the simulated Particles
  if (m_cfg.simulatedParticlesCollection != "" && m_cfg.particleWriter) {
    // retrieve it
    const std::vector<Acts::ParticleProperties>* simulatedParticles = nullptr;
    // read and go
    if (eventStore
        && eventStore->get(m_cfg.simulatedParticlesCollection,
                           simulatedParticles)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not read colleciton of simulated particles.");
      return FW::ProcessCode::ABORT;
    }
    // run over it - and write them
    // write to file if you have
    if (m_cfg.particleWriter->write(*simulatedParticles)
        == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not write colleciton of simulated particles.");
      return FW::ProcessCode::ABORT;
    }
  }

  // write the clusters
  if (m_cfg.planarClustersCollection != "" && m_cfg.planarClusterWriter) {
    // retrieve the input data
    const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>*
        planarClusters
        = nullptr;
    // read and go
    if (eventStore
        && eventStore->get(m_cfg.planarClustersCollection, planarClusters)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not read colleciton of clusters.");
      return FW::ProcessCode::ABORT;
    }
    // run over it - and write them
    if (m_cfg.planarClusterWriter->write(*planarClusters)
        == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not write colleciton of clusters.");
      return FW::ProcessCode::ABORT;
    }
  }

  // write the space points
  if (m_cfg.spacePointCollection != "" && m_cfg.spacePointWriter) {
    // retrieve the input data
    const FW::DetectorData<geo_id_value, Acts::Vector3D>* spacePoints = nullptr;
    // read and go
    if (eventStore
        && eventStore->get(m_cfg.spacePointCollection, spacePoints)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not read colleciton of space points.");
      return FW::ProcessCode::ABORT;
    }
    // run over it - and write them
    if (m_cfg.spacePointWriter->write(*spacePoints) == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not cwrtie olleciton of space points.");
      return FW::ProcessCode::ABORT;
    }
  }

  return FW::ProcessCode::SUCCESS;
}
