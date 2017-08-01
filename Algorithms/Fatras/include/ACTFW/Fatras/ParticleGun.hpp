#ifndef ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H
#define ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H 

#include <mutex>
#include <array>
#include "ACTFW/Readers/IReaderT.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class RandomNumbersSvc;
}

namespace FWE {

/// @class ParticleGun
///
/// Particle gun implementation of the IParticleReader. It can be used as
/// particle input for the Fatras example.
/// It generates particles of given type with random momentum and random vertex
/// in a given range. It fills a vector of particle properties for feeding into
/// fast simulation.
///
class ParticleGun : public FW::IReaderT<std::vector<Acts::ParticleProperties>>
{
public:
  /// @class Config
  /// configuration struct
  class Config
  {
  public:
    /// The name of the reader
    std::string name = "ParticleGun";
    // FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    /// number of particles
    size_t nParticles = 0;
    /// low, high for eta range
    std::array<double, 2> etaRange = {{-3., 3.}};
    /// low, high for phi range
    std::array<double, 2> phiRange = {{-M_PI, M_PI}};
    /// low, high for pt range
    std::array<double, 2> ptRange = {{100., 10000.}};
    /// the mass of the particle
    double mass = 0.;
    /// the charge of the particle
    double charge = 0.;
    /// the pdg type of the particle
    pdg_type pID = 0.;

    Config() {}
  };

  /// Constructor
  /// @param cfg is the configuration class
  ParticleGun(const Config&                       cfg,
              std::unique_ptr<const Acts::Logger> logger
              = Acts::getDefaultLogger("ParticleGun", Acts::Logging::INFO));

  /// Destructor
  virtual ~ParticleGun();

  // clang-format off
  /// @copydoc FW::IReaderT::read(std::vector<Acts::ParticleProperties>&,size_t,const FW::AlgorithmContext*)
  // clang-format on
  FW::ProcessCode
  read(std::vector<Acts::ParticleProperties>& particleProperties,
       size_t                                 skip    = 0,
       const FW::AlgorithmContext*            context = nullptr) override final;

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

  /// the configuration class
  Config m_cfg;
  /// logger instance
  std::unique_ptr<const Acts::Logger> m_logger;
};

const std::string&
ParticleGun::name() const
{
  return m_cfg.name;
}
}

#endif  // ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H
