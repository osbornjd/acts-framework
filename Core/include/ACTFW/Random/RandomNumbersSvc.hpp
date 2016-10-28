//
//  RandomNumbersSvc.hpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#ifndef ACTFW_RANDOM_RANDOMNUMBERSSVC_H
#define ACTFW_RANDOM_RANDOMNUMBERSSVC_H 1

#include <array>
#include <memory>
#include <random>
#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

///  @enum class Distribution
enum class Distribution { uniform, gauss, landau, gamma };

/// @class RandomNumbersSvc
///
/// An implementation of the std random numbers
///
typedef std::mt19937                     RandomEngine;  // Mersenne Twister
typedef std::normal_distribution<double> GaussDist;     // Normal Distribution
typedef std::uniform_real_distribution<double>
                                        UniformDist;  // Uniform Distribution
typedef std::gamma_distribution<double> GammaDist;    // Gamma Distribution

class RandomNumbersSvc : public IService
{
public:
  /// @class Config
  ///
  /// Nested Configuration class
  struct Config
  {
    /// default logger
    std::shared_ptr<Acts::Logger> logger;
    /// service name
    std::string name;
    /// random seed
    unsigned int seed = 1234567890;
    /// configuration uniform
    std::array<double, 2> uniform_parameters = {{0, 1}};
    /// configuration gauss
    std::array<double, 2> gauss_parameters = {{0, 1}};
    /// configuration landau
    std::array<double, 2> landau_parameters = {{0, 1}};
    /// configuration gamma
    std::array<double, 2> gamma_parameters = {{0, 1}};

	Config(const std::string&   lname = "RandomNumbersSvc",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger{Acts::getDefaultLogger(lname, lvl)}
      , name{lname}
    {
    }
  };

  /// @class Generator
  ///
  /// A random number generator
  class Generator
  {
  public:
    /// Initialize a generator
    ///
    /// @param cfg is the host's configuration
    /// @param seed is the seed to use for this generator
    Generator(const Config & cfg,
              unsigned int seed);

    /// Draw a random number
    ///
    /// @param dPar is the distribution to draw from
    double
    draw(Distribution dPar);

  private:
    const Config & m_cfg;      ///< link to host configuration
    RandomEngine   m_engine;   ///< random engine
    GaussDist      m_gauss;    ///< gauss distribution
    UniformDist    m_uniform;  ///< uniform distribution
    GammaDist      m_gamma;    ///< gamma distribution
  };

  /// Constructor
  RandomNumbersSvc(const Config& cfg);

  // Framework initialize method
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

  /// Spawn an algorithm-local random number generator
  ///
  /// @param context is the AlgorithmContext of the host algorithm
  Generator
  spawnGenerator(const AlgorithmContext & context) const;

  /// Draw a random number in a thread-unsafe way
  ///
  /// @param dPar is the distribution to draw from
  double
  draw(Distribution dPar);

  /// Ask for the seed
  unsigned int
  seed() const
  {
    return m_cfg.seed;
  }

  /// Framework name() method
  const std::string&
  name() const override final;

private:
  Config                        m_cfg;      ///< the configuration class
  Generator                     m_rng;      ///< default generator

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

inline const std::string&
RandomNumbersSvc::name() const
{
  return m_cfg.name;
}

}

#endif  // ACTFW_RANDOM_RANDOMNUMBERSSVC_H
