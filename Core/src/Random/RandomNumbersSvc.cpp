//
//  RandomNumbersSvc.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Random/LandauQuantile.hpp"

FW::RandomNumbersSvc::RandomNumbersSvc(const FW::RandomNumbersSvc::Config& cfg)
  : m_cfg(cfg), m_rng{m_cfg, m_cfg.seed}
{
}

FW::RandomNumbersSvc::Generator::Generator(const Config& cfg, unsigned int seed)
  : m_cfg{cfg}
  , m_engine{seed}
  , m_gauss{m_cfg.gauss_parameters[0], m_cfg.gauss_parameters[1]}
  , m_uniform{m_cfg.uniform_parameters[0], m_cfg.uniform_parameters[1]}
  , m_gamma{m_cfg.gamma_parameters[0], m_cfg.gamma_parameters[1]}
  , m_poisson(m_cfg.poisson_parameter)
{
}

FW::ProcessCode
FW::RandomNumbersSvc::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::RandomNumbersSvc::finalize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::RandomNumbersSvc::Generator
FW::RandomNumbersSvc::spawnGenerator(const AlgorithmContext& context) const
{
  const auto         eventContext = context.eventContext;
  const unsigned int generatorID
      = context.algorithmNumber * eventContext->jobContext->eventCount
      + eventContext->eventNumber;
  return Generator(m_cfg, m_cfg.seed + generatorID);
}

double
FW::RandomNumbersSvc::Generator::draw(FW::Distribution dPar)
{
  // switch and return
  switch (dPar) {
  case Distribution::gauss:
    return m_gauss(m_engine);
  case Distribution::uniform:
    return m_uniform(m_engine);
  case Distribution::gamma:
    return m_gamma(m_engine);
  case Distribution::landau: {
    double x   = m_uniform(m_engine);
    double res = m_cfg.landau_parameters[0]
        + landau_quantile(x, m_cfg.landau_parameters[1]);
    return res;
  }
  case Distribution::poisson:
    return m_poisson(m_engine);
  }
  return 0.;
}

double
FW::RandomNumbersSvc::drawGauss()
{
  return m_rng.draw(Distribution::gauss);
}

double
FW::RandomNumbersSvc::drawUniform()
{
  return m_rng.draw(Distribution::uniform);
}
double
FW::RandomNumbersSvc::drawLandau()
{
  return m_rng.draw(Distribution::landau);
}
double
FW::RandomNumbersSvc::drawGamma()
{
  return m_rng.draw(Distribution::gamma);
}
double
FW::RandomNumbersSvc::drawPoisson()
{
  return m_rng.draw(Distribution::poisson);
}
