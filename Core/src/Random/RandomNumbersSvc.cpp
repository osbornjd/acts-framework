//
//  RandomNumbersSvc.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Random/LandauQuantile.hpp"


FW::LandauDist::LandauDist(const Config& cfg)
  : m_cfg(cfg)
  , m_uniform{m_cfg.uniform_parameters[0], m_cfg.uniform_parameters[1]}
{
}

double
FW::LandauDist::operator()(RandomEngine& engine)
{
  double x   = m_uniform(engine);
  double res = m_cfg.landau_parameters[0]
      + landau_quantile(x, m_cfg.landau_parameters[1]);
  return res;
}

FW::RandomNumbersSvc::RandomNumbersSvc(const FW::RandomNumbersSvc::Config& cfg)
  : m_cfg(cfg)
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

FW::RandomEngine
FW::RandomNumbersSvc::spawnGenerator(const AlgorithmContext& context) const
{
  const auto         eventContext = context.eventContext;
  const unsigned int generatorID
      = context.algorithmNumber * eventContext->jobContext->eventCount
      + eventContext->eventNumber;
  return RandomEngine(m_cfg.seed + generatorID);
}
