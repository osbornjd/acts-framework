//  IExtrapolationCellWriter.h
//  ACTS-Development
//
//  Created by Andreas Salzburger on 23/05/16.
//
//
#ifndef ACTFW_ROOTPYTHIA8_TPYTHIA8GENERATOR_H
#define ACTFW_ROOTPYTHIA8_TPYTHIA8GENERATOR_H

#include <mutex>
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

class TPythia8;

namespace FWRoot {

/// @class IParticleReader
///
/// Interface class that fills a vector of particle
/// proerties for feeding into the fast simulation
///
class TPythia8Generator
  : public FW::IReaderT<std::vector<Acts::ParticleProperties>>
{
public:
  /// @class Config
  /// configuration struct
  class Config
  {
  public:
    int                      pdgBeam0  = 2212;  ///< pdg code of incoming beam 1
    int                      pdgBeam1  = 2212;  ///< pdg code of incoming beam 2
    double                   cmsEnergy = 14000.;  ///< center of mass energy
    std::vector<std::string> processStrings
        = {{"HardQCD:all = on"}};  ///< pocesses
    std::string name = "TPythia8Generator";

    Config() {}
  };

  /// Constructor
  /// @param cfg is the configuration class
  /// @param logger is the logger instance
  TPythia8Generator(const Config&                       cfg,
                    std::unique_ptr<const Acts::Logger> logger
                    = Acts::getDefaultLogger("TPythia8Generator",
                                             Acts::Logging::INFO));

  /// Destructor
  virtual ~TPythia8Generator();

  std::string
  name() const override final;

  // clang-format off
  /// @copydoc FW::IReaderT::read(std::vector<Acts::ParticleProperties>&,size_t,const FW::AlgorithmContext*)
  // clang-format on
  FW::ProcessCode
  read(std::vector<Acts::ParticleProperties>& pProperties,
       size_t                                 skip = 0,
       const FW::AlgorithmContext* context = nullptr) override final;

  /// Reads in a  list of paritlces
  /// @return is a process code indicateing if the reading succeeded
  FW::ProcessCode
  initialize() override final;

  /// Reads in a  list of paritlces
  /// @return is a process code indicateing if the reading succeeded
  FW::ProcessCode
  finalize() override final;

private:
  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return (*m_logger);
  }

  /// the configuration class
  Config m_cfg;
  /// the pythia object
  TPythia8* m_pythia8;
  /// logger instance
  std::unique_ptr<const Acts::Logger> m_logger;
  /// mutex used to protect multi-threaded reads
  std::mutex m_read_mutex;
};

}  // namespace FWRoot

#endif  // ACTFW_ROOTPYTHIA8_TPYTHIA8GENERATOR_H
