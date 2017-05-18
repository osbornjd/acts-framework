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
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/IOAlgorithm.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTFW/Writers/IEventDataWriterT.hpp"

namespace Acts {
  class PlanarModuleCluster;
  class ParticleProperties;
}

namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
class BarcodeSvc;
}


namespace FWA {


/// @class FatrasWriteAlgorithm
///
/// FatrasWriteAlgorithm to read EvGen from some input
/// Allows for pile-up reading as well 
class FatrasWriteAlgorithm : public FW::IOAlgorithm
{

public:

  /// @struct Config
  /// configuration struct for this Algorithm
  struct Config {
    
    /// name of the particle collection
    std::string                            simulatedParticlesCollection  = "SimulatedParticles";
    /// particle writer
    std::shared_ptr<FW::IWriterT<std::vector<Acts::ParticleProperties>> > particleWriter        = nullptr;
    // name of the space point collection
    std::string                            spacePointCollection          = "SpacePoints";
    // write out the planar clusters
    std::shared_ptr<FW::IEventDataWriterT<Acts::Vector3D> > spacePointWriter = nullptr;
    // name of the cluster collection
    std::string                            planarClustersCollection      = "PlanarClusters";
    // write out the planar clusters
    std::shared_ptr<FW::IEventDataWriterT<Acts::PlanarModuleCluster> > planarClusterWriter = nullptr;
    /// the job WhiteBoard
    std::shared_ptr<FW::WhiteBoard>        jBoard                        = nullptr;
    /// the name of the algorithm
    std::string name = "FatrasWriteAlgorithm";
      
    Config()
    {
    }
    
  };

  /// Constructor
  FatrasWriteAlgorithm(
      const Config&                 cnf,
      std::unique_ptr<Acts::Logger> logger
      = Acts::getDefaultLogger("FatrasWriteAlgorithm",
                               Acts::Logging::INFO));

  /// Virtual destructor
  virtual ~FatrasWriteAlgorithm() {}
  
  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr)
  override final;

  /// Skip a few events in the IO stream
  FW::ProcessCode
  skip(size_t nEvents = 1)
  override final;

  /// Read out data from the input stream
  FW::ProcessCode
  read(const FW::AlgorithmContext context) const
  override final;

  /// Write data to the output stream
  FW::ProcessCode
  write(const FW::AlgorithmContext context) const
  override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize()
  override final;

  /// Framework name() method
  const std::string&
  name() const override final;

  /// return the jobStore - things that live for the full job
  std::shared_ptr<FW::WhiteBoard>
  jobStore() const override final;
  
protected:
  Config                        m_cfg;
  std::unique_ptr<Acts::Logger> m_logger;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
  
  
};

inline FW::ProcessCode
FatrasWriteAlgorithm::read(const FW::AlgorithmContext) const
{
  return FW::ProcessCode::SUCCESS;
}

inline std::shared_ptr<FW::WhiteBoard>
FatrasWriteAlgorithm::jobStore() const
{
  return m_cfg.jBoard;
}

inline const std::string&
FatrasWriteAlgorithm::name() const
{
  return m_cfg.name;
}

}

#endif  /// ACTFW_ALGORITHMS_FATRAS_WRITEALGORITHM_H
