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
#include "ACTFW/Random/LandauQuantile.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// @class RandomNumbersSvc
///
/// This service provides Algorithm-local random number generators, allowing for
/// thread-safe, lock-free and reproducible random number generations in both
/// single-threaded and multi-threaded test framework runs.
///
/// The following random number generator ("engine") is used:
///
typedef std::mt19937                     RandomEngine;  ///< Mersenne Twister
///
/// The following standard random number distributions are supported...
///
typedef std::normal_distribution<double> GaussDist;     ///< Normal Distribution
typedef std::uniform_real_distribution<double>
                                        UniformDist;  ///< Uniform Distribution
typedef std::gamma_distribution<double> GammaDist;    ///< Gamma Distribution
typedef std::poisson_distribution<int>  PoissonDist;  ///< Poisson Distribution
///
/// ...and, in addition, the Landau distribution is provided
///
class LandauDist
{
public:
  /// RandomNumberDistributions should provide a parameters struct
  struct param_type
  {
    double mean = 0.;
    double scale = 1.;
    param_type(double mean, double scale);
  };

  /// There should be a constructor from raw parameters and a parameters struct
  LandauDist(double mean, double scale);
  LandauDist(const param_type& cfg);

  /// RandomNumberDistributions should provide a result type typedef
  using result_type = double;

  /// Generate a random number following a Landau distribution
  template<typename Generator>
  double
  operator()(Generator& engine) {
    double x   = std::generate_canonical<float, 10>(engine);
    double res = m_cfg.mean + landau_quantile(x, m_cfg.scale);
    return res;
  }

private:
  param_type m_cfg; ///< configuration struct
};
///
/// The role of the RandomNumbersSvc is only to spawn Algorithm-local random
/// number generators. Clients should spawn their own local distributions
/// whenever needed.
///
class RandomNumbersSvc : public IService
{
public:
  /// @class Config
  ///
  /// Nested Configuration class
  struct Config
  {
    /// default logger
    std::shared_ptr<const Acts::Logger> logger;
    /// service name
    std::string name;
    /// random seed
    unsigned int seed = 1234567890;

    Config(const std::string&   lname = "RandomNumbersSvc",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger{Acts::getDefaultLogger(lname, lvl)}, name{lname}
    {
    }
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
  RandomEngine
  spawnGenerator(const AlgorithmContext& context) const;

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
  Config    m_cfg;  ///< the configuration class

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
