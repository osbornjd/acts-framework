//
//  RandomNumbersSvc.hpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#ifndef ACTFW_RANDOM_RANDOMNUMBERSSVC_H
#define ACTFW_RANDOM_RANDOMNUMBERSSVC_H 1

#include <memory>
#include <random>
#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

/// @class RandomNumbersSvc
///
/// This service provides Algorithm-local random number generators, allowing for
/// thread-safe, lock-free and reproducible random number generation across
/// single-threaded and multi-threaded test framework runs.
///
/// The following random number generator ("engine") is used:
///
using RandomEngine = std::mt19937;  ///< Mersenne Twister
///
/// The role of the RandomNumbersSvc is only to spawn Algorithm-local random
/// number generators ("engines"). It does not, in and of itself, accomodate
/// requests for specific random number distributions (uniform, gaussian, etc).
///
/// For this purpose, clients should spawn their own local distribution objects
/// as needed, following the C++11 STL design. See RandomNumberDistributions.hpp
/// for some examples of distributions that can be used.
///
class RandomNumbersSvc : public IService
{
public:
  struct Config
  {
    unsigned int seed = 1234567890;  ///< random seed
  };

  /// Constructor
  RandomNumbersSvc(const Config&                       cfg,
                   std::unique_ptr<const Acts::Logger> logger
                   = Acts::getDefaultLogger("RandomNumbersSvc",
                                            Acts::Logging::INFO));

  /// Framework name() method
  std::string
  name() const override final;

  // Framework initialize method
  FW::ProcessCode
  initialize() override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

  /// Spawn an algorithm-local random number generator. To avoid inefficiencies
  /// and multiple uses of a given RNG seed, this should only be done once per
  /// Algorithm invocation, after what the generator object should be reused.
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

private:
  Config                              m_cfg;  ///< the configuration class
  std::unique_ptr<const Acts::Logger> m_logger;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_logger;
  }
};

}  // namespace FW

#endif  // ACTFW_RANDOM_RANDOMNUMBERSSVC_H
