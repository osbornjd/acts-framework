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
  : m_cfg(cfg)
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
FW::RandomNumbersSvc::Generator::drawGauss()
{
  return m_gauss(m_engine);
}

double
FW::RandomNumbersSvc::Generator::drawUniform()
{
  return m_uniform(m_engine);
}

double
FW::RandomNumbersSvc::Generator::drawLandau()
{
  double x   = m_uniform(m_engine);
  double res = m_cfg.landau_parameters[0]
      + landau_quantile(x, m_cfg.landau_parameters[1]);
  return res;
}

double
FW::RandomNumbersSvc::Generator::drawGamma()
{
  return m_gamma(m_engine);
}

double
FW::RandomNumbersSvc::Generator::drawPoisson()
{
  return m_poisson(m_engine);
}
