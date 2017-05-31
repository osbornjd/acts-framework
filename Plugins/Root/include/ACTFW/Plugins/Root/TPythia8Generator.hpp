//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_ROOTPYTHIA8_TPYTHIA8GENERATOR_H
#define ACTFW_ROOTPYTHIA8_TPYTHIA8GENERATOR_H

#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTFW/Readers/IReaderT.hpp"
#include <mutex>

class TPythia8;

namespace FWRoot {

/// @class IParticleReader
///
/// Interface class that fills a vector of particle
/// proerties for feeding into the fast simulation   
///
class TPythia8Generator 
  : public FW::IReaderT< std::vector<Acts::ParticleProperties> >
{
public:
  /// @class Config
  /// configuration struct 
  class Config
  {
  public:
      int                       pdgBeam0  = 2212;   ///< pdg code of incoming beam 1
      int                       pdgBeam1  = 2212;   ///< pdg code of incoming beam 2
      double                    cmsEnergy = 14000.; ///< center of mass energy
      std::vector<std::string>  processStrings = {{"HardQCD:all = on"}}; ///< pocesses
      std::string               name           = "TPythia8Generator";
      
      
      Config()
      {
      }
  };

  /// Constructor
  /// @param cfg is the configuration class
  /// @param logger is the logger instance
  TPythia8Generator(const Config& cfg,
                    std::unique_ptr<const Acts::Logger> logger
                    = Acts::getDefaultLogger("TPythia8Generator", Acts::Logging::INFO));

  /// Destructor
  virtual
  ~TPythia8Generator();
  
  /// Reads in a  list of paritlces
  /// for the fast simulation 
  /// @param pProperties is the vector to be read in
  /// @return is a process code indicateing if the reading succeeded
  FW::ProcessCode
  read(std::vector<Acts::ParticleProperties>& pProperties, size_t skip=0) override final;
  
  /// Reads in a  list of paritlces
  /// @return is a process code indicateing if the reading succeeded
  FW::ProcessCode
  initialize() override final;
    
  /// Reads in a  list of paritlces
  /// @return is a process code indicateing if the reading succeeded
  FW::ProcessCode
  finalize() override final;
  
  /// Reads in a  list of paritlces
  /// @return is a process code indicateing if the reading succeeded
  const std::string&
  name() const override final;
  
    
private:
  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
     return (*m_logger);
  }
    
  Config                        m_cfg;     ///< the configuration class
  TPythia8*                     m_pythia8; ///< the pythia object
  std::unique_ptr<const Acts::Logger> m_logger;  ///< logger instance
  std::mutex                    m_read_mutex; ///< mutex used to protect multi-threaded reads
    
};

const std::string&
TPythia8Generator::name() const
{
  return m_cfg.name;
}

}

#endif  // ACTFW_ROOTPYTHIA8_TPYTHIA8GENERATOR_H
