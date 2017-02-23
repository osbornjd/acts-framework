//
//  IOAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_FATRAS_WRITEALGORITHM_H
#define ACTFW_FATRAS_WRITEALGORITHM_H 1

#include <memory>
#include <string>
#include "ACTS/Utilities/Logger.hpp"
#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/IOAlgorithm.hpp"
#include "ACTFW/Writers/IParticlePropertiesWriter.hpp"
#include "ACTFW/Writers/IPlanarClusterWriter.hpp"

namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
class BarcodeSvc;
}


namespace FWE {


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
    std::shared_ptr<FW::IParticlePropertiesWriter> particleWriter        = nullptr;
    // name of the cluster collection
    std::string                            planarClustersCollection      = "PlanarClusters";
    // 
    std::shared_ptr<FW::IPlanarClusterWriter> planarClusterWriter        = nullptr;
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
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr);

  /// Skip a few events in the IO stream
  virtual FW::ProcessCode
  skip(size_t nEvents = 1);

  /// Read out data from the input stream
  virtual FW::ProcessCode
  read(const FW::AlgorithmContext context) const;

  /// Write data to the output stream
  virtual FW::ProcessCode
  write(const FW::AlgorithmContext context) const;

  /// Framework finalize mehtod
  virtual FW::ProcessCode
  finalize();

  /// Framework name() method
  virtual const std::string&
  name() const;

  /// return the jobStore - things that live for the full job
  virtual std::shared_ptr<FW::WhiteBoard>
  jobStore() const;
  
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

#endif  /// ACTFW_FATRAS_WRITEALGORITHM_H
