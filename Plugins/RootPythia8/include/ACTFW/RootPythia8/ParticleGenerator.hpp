//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//

#ifndef ACTFW_ROOTPYTHIA8_PARTICLEGENERATOR_H
#define ACTFW_ROOTPYTHIA8_PARTICLEGENERATOR_H 1

#include "ACTS/Utilities/Logger.hpp"
#include "ACTFW/Readers/IParticleReader.hpp"

class TPythia8;

namespace FWRootPythia8 {

/// @class IParticleReader
///
/// Interface class that fills a vector of particle
/// proerties for feeding into the fast simulation   
///
class ParticleGenerator : public FW::IParticleReader
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
      
      Config()
      {
      }
  };

  /// Constructor
  /// @param cfg is the configuration class
  ParticleGenerator(const Config& cfg,
                    std::unique_ptr<Acts::Logger> logger
                    = Acts::getDefaultLogger("ParticleGenerator", Acts::Logging::INFO));

  /// Destructor
  virtual
  ~ParticleGenerator();
  
  /// returns the list of particles to be processed
  /// for the fast simulation 
  std::vector< Acts::ParticleProperties > 
  particles() const final;
  
  /// skip some events
  void
  skip(size_t nEvents = 1) const final;
  
private:
  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
     return (*m_logger);
  }
    
  Config                        m_cfg;     ///< the configuration class
  TPythia8*                     m_pythia8; ///< the pythia object
  std::unique_ptr<Acts::Logger> m_logger;  ///< logger instance
    
};
}

#endif  // ACTFW_ROOTPYTHIA8_PARTICLEGENERATOR_H
