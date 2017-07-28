//
//  IOAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_ALGORITHMS_FATRAS_WRITEALGORITHM_H
#define ACTFW_ALGORITHMS_FATRAS_WRITEALGORITHM_H

#include <memory>
#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/IWriter.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IEventDataWriterT.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
class PlanarModuleCluster;
class ParticleProperties;
}  // namespace Acts
namespace FW {
class RandomNumbersSvc;
class BarcodeSvc;
}  // namespace FW

namespace FWA {

/// @class FatrasWriteAlgorithm
///
/// FatrasWriteAlgorithm to read EvGen from some input
/// Allows for pile-up reading as well
class FatrasWriteAlgorithm : public FW::IWriter
{
public:
  struct Config
  {
    /// name of the particle collection
    std::string simulatedParticlesCollection = "SimulatedParticles";
    /// particle writer
    std::shared_ptr<FW::IWriterT<std::vector<Acts::ParticleProperties>>>
        particleWriter = nullptr;
    // name of the space point collection
    std::string spacePointCollection = "SpacePoints";
    // write out the planar clusters
    std::shared_ptr<FW::IEventDataWriterT<Acts::Vector3D>> spacePointWriter
        = nullptr;
    // name of the cluster collection
    std::string planarClustersCollection = "PlanarClusters";
    // write out the planar clusters
    std::shared_ptr<FW::IEventDataWriterT<Acts::PlanarModuleCluster>>
        planarClusterWriter = nullptr;
  };

  /// Constructor
  FatrasWriteAlgorithm(const Config&                       cnf,
                       std::unique_ptr<const Acts::Logger> logger
                       = Acts::getDefaultLogger("FatrasWriteAlgorithm",
                                                Acts::Logging::INFO));

  /// Virtual destructor
  virtual ~FatrasWriteAlgorithm() {}

  /// Framework name() method
  std::string
  name() const final;

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

  /// Write data to the output stream
  FW::ProcessCode
  write(const FW::AlgorithmContext& context) final;

private:
  Config                              m_cfg;
  std::unique_ptr<const Acts::Logger> m_logger;

  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FWA

#endif  /// ACTFW_ALGORITHMS_FATRAS_WRITEALGORITHM_H
