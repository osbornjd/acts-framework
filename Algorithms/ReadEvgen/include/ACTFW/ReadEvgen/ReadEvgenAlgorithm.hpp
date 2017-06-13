//
//  IOAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_EXAMPLES_READEVGENALGORITHM_H
#define ACTFW_EXAMPLES_READEVGENALGORITHM_H 1

#include <memory>
#include <string>
#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/IOAlgorithm.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"

namespace FW {
class WhiteBoard;
class RandomNumbersSvc;
class BarcodeSvc;
}


namespace FWA {

/// @class ReadEvgenAlgorithm
///
/// ReadEvgenAlgorithm to read EvGen from some input
/// Allows for pile-up reading as well 
class ReadEvgenAlgorithm : public FW::IOAlgorithm
{

public:
  /// @struct Config
  /// configuration struct for this Algorithm
  struct Config {
    
    /// name of the output collection
    std::string                            evgenParticlesCollection  = "EvgenParticles";
    /// the hard scatter reader
    std::shared_ptr< FW::IReaderT< std::vector<Acts::ParticleProperties > > > 
        hardscatterParticleReader = nullptr;
    /// the pileup reader       
    std::shared_ptr< FW::IReaderT< std::vector<Acts::ParticleProperties> > > 
        pileupParticleReader      = nullptr;
    /// the number of pileup events
    std::shared_ptr<FW::RandomNumbersSvc>  pileupRandomNumbers       = nullptr;
    std::shared_ptr<FW::RandomNumbersSvc>  pileupVertexDistT         = nullptr;
    std::shared_ptr<FW::RandomNumbersSvc>  pileupVertexDistZ         = nullptr;
    /// the BarcodeSvc
    std::shared_ptr<FW::BarcodeSvc>        barcodeSvc                = nullptr;
    /// output writer
    std::shared_ptr<FW::IWriterT< std::vector<Acts::ParticleProperties> > > 
        particleWriter    = nullptr;
    /// the job WhiteBoard
    std::shared_ptr<FW::WhiteBoard>        jBoard                    = nullptr;
    /// the name of the algorithm
    std::string name = "Algorithm";
      
    Config()
    {
    }
    
  };

  /// Constructor
  ReadEvgenAlgorithm(
      const Config&                       cnf,
      std::unique_ptr<const Acts::Logger> logger
      = Acts::getDefaultLogger("ReadEvgenAlgorithm",
                               Acts::Logging::INFO));

  /// Virtual destructor
  virtual ~ReadEvgenAlgorithm() {}
  
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
  Config                              m_cfg;
  std::unique_ptr<const Acts::Logger> m_logger;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
  
  
};

inline FW::ProcessCode
ReadEvgenAlgorithm::write(const FW::AlgorithmContext) const
{
  return FW::ProcessCode::SUCCESS;
}

inline std::shared_ptr<FW::WhiteBoard>
ReadEvgenAlgorithm::jobStore() const
{
  return m_cfg.jBoard;
}

inline const std::string&
ReadEvgenAlgorithm::name() const
{
  return m_cfg.name;
}

}

#endif  /// ACTFW_EXAMPLES_READEVGENALGORITHM_H
