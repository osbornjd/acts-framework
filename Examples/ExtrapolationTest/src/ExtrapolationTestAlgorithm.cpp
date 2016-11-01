#include <iostream>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IExtrapolationCellWriter.hpp"
#include "ACTS/EventData/NeutralParameters.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTFW/ExtrapolationTest/ExtrapolationTestAlgorithm.hpp"

FWE::ExtrapolationTestAlgorithm::ExtrapolationTestAlgorithm(
    const FWE::ExtrapolationTestAlgorithm::Config& cfg,
    std::unique_ptr<Acts::Logger>                  logger)
  : FW::Algorithm(cfg, std::move(logger)), m_cfg(cfg)
{
}

FWE::ExtrapolationTestAlgorithm::~ExtrapolationTestAlgorithm()
{
}

/** Framework finalize mehtod */
FW::ProcessCode
FWE::ExtrapolationTestAlgorithm::initialize(
    std::shared_ptr<FW::WhiteBoard> jStore)
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
FWE::ExtrapolationTestAlgorithm::execute(const FW::AlgorithmContext context) const
{
  // Create a random number generator
  FW::RandomNumbersSvc::Generator rng =
    m_cfg.randomNumbers->spawnGenerator(context);

  // loop
  for (size_t iex = 0; iex < m_cfg.testsPerEvent; ++iex) {
    // gaussian d0 and z0
    double d0    = drawGauss(rng, m_cfg.d0Defs);
    double z0    = drawGauss(rng, m_cfg.z0Defs);
    double phi   = drawUniform(rng, m_cfg.phiRange);
    double eta   = drawUniform(rng, m_cfg.etaRange);
    double theta = 2. * atan(exp(-eta));
    double pt    = drawUniform(rng, m_cfg.ptRange);
    double p     = pt / sin(theta);
    double q     = drawUniform(rng, {{0., 1.}}) > 0.5 ? 1. : -1.;

    Acts::Vector3D momentum(
        p * sin(theta) * cos(phi), p * sin(theta) * sin(phi), p * cos(theta));
    std::unique_ptr<Acts::ActsSymMatrixD<5>> cov;
    Acts::ActsVectorD<5>                     pars;
    pars << d0, z0, phi, theta, q / p;
    // perigee parameters
    ACTS_VERBOSE("Building parameters from Perigee with (" << d0 << ", " << z0
                                                           << ", "
                                                           << phi
                                                           << ", "
                                                           << theta
                                                           << ", "
                                                           << q / p);
    // charged extrapolation
    Acts::PerigeeSurface pSurface(Acts::Vector3D(0., 0., 0.));

    // neutral extrapolation
    if (m_cfg.parameterType) {
      Acts::BoundParameters startParameters(
          std::move(cov), std::move(pars), pSurface);
      if (executeTestT<Acts::TrackParameters>(startParameters)
          != FW::ProcessCode::SUCCESS)
        ACTS_WARNING("Test of parameter extrapolation did not succeed.");

    } else {
      // charged extrapolation
      Acts::NeutralBoundParameters startParameters(
          std::move(cov), std::move(pars), pSurface);
      if (executeTestT<Acts::NeutralParameters>(startParameters)
          != FW::ProcessCode::SUCCESS)
        ACTS_WARNING("Test of parameter extrapolation did not succeed.");
    }
  }
  // return SUCCESS to the frameword
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::ExtrapolationTestAlgorithm::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

double
FWE::ExtrapolationTestAlgorithm::drawGauss(
    FW::RandomNumbersSvc::Generator& rng,
    const std::array<double, 2> & pars) const
{
  double mean  = pars[0];
  double sigma = pars[1];
  return mean + rng.draw(FW::Distribution::gauss) * sigma;
}

double
FWE::ExtrapolationTestAlgorithm::drawUniform(
    FW::RandomNumbersSvc::Generator& rng,
    const std::array<double, 2>& range) const
{
  double low   = range[0];
  double high  = range[1];
  double delta = high - low;
  return low + rng.draw(FW::Distribution::uniform) * delta;
}
