//
//  RandomNumbersSvc.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#include "ACTFW/Random/RandomNumbersSvc.hpp"


FW::LandauDist::param_type::param_type(double mean, double scale)
  : mean(mean)
  , scale(scale)
{
}

FW::LandauDist::LandauDist(double mean, double scale)
  : m_cfg(mean, scale)
{
}

FW::LandauDist::LandauDist(const param_type& cfg)
  : m_cfg(cfg)
{
}

FW::RandomNumbersSvc::RandomNumbersSvc(const Config& cfg)
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
