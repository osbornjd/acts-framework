#include "RandomNumbersAlgorithm.hpp"

#include <iostream>

#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

FWE::RandomNumbersAlgorithm::RandomNumbersAlgorithm(
    const FWE::RandomNumbersAlgorithm::Config& cfg,
    std::unique_ptr<const Acts::Logger>        logger)
  : FW::Algorithm(cfg, std::move(logger)), m_cfg(cfg)
{
}

FWE::RandomNumbersAlgorithm::~RandomNumbersAlgorithm()
{
}

/** Framework finalize mehtod */
FW::ProcessCode
FWE::RandomNumbersAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

/** Framework execode method */
FW::ProcessCode
FWE::RandomNumbersAlgorithm::execute(const FW::AlgorithmContext context) const
{
  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(context);

  // Spawn some random number distributions
  FW::GaussDist gaussDist(m_cfg.gaussParameters[0], m_cfg.gaussParameters[1]);
  FW::UniformDist uniformDist(m_cfg.uniformParameters[0],
                              m_cfg.uniformParameters[1]);
  FW::LandauDist landauDist(m_cfg.landauParameters[0],
                            m_cfg.landauParameters[1]);
  FW::GammaDist gammaDist(m_cfg.gammaParameters[0], m_cfg.gammaParameters[1]);
  FW::PoissonDist poissonDist(m_cfg.poissonParameter);

  for (size_t idraw = 0; idraw < m_cfg.drawsPerEvent; ++idraw) {
    double gauss   = gaussDist(rng);
    double uniform = uniformDist(rng);
    double landau  = landauDist(rng);
    double gamma   = gammaDist(rng);
    int    poisson = poissonDist(rng);

    ACTS_VERBOSE("Gauss   : " << gauss);
    ACTS_VERBOSE("Uniform : " << uniform);
    ACTS_VERBOSE("Landau  : " << landau);
    ACTS_VERBOSE("Gamma   : " << gamma);
    ACTS_VERBOSE("Poisson : " << poisson);
  }
  return FW::ProcessCode::SUCCESS;
}

/** Framework finalize mehtod */
FW::ProcessCode
FWE::RandomNumbersAlgorithm::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}
