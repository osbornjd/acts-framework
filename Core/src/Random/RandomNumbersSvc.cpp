//
//  RandomNumbersSvc.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#include "ACTFW/Random/RandomNumbersSvc.hpp"

FW::RandomNumbersSvc::RandomNumbersSvc(
    const Config&                       cfg,
    std::unique_ptr<const Acts::Logger> logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

std::string
FW::RandomNumbersSvc::name() const
{
  return "RandomNumbersSvc";
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
  // use Cantor pairing function to generate a unique generator id from
  // algorithm and event number to get a consistent seed
  // see https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
  const unsigned int k1 = context.algorithmNumber;
  const unsigned int k2 = context.eventNumber;
  const unsigned int id = (k1 + k2) * (k1 + k2 + 1) / 2 + k2;
  return RandomEngine(m_cfg.seed + id);
}
