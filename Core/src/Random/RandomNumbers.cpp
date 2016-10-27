//
//  RandomNumbers.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#include "ACTFW/Random/RandomNumbers.hpp"
#include "ACTFW/Random/LandauQuantile.hpp"

FW::RandomNumbers::RandomNumbers(const FW::RandomNumbers::Config& cfg,
                                 std::unique_ptr<Acts::Logger>    logger)
  : m_cfg(cfg)
  , m_logger(std::move(logger))
  , m_rng{ m_cfg, m_cfg.seed }
{
}

FW::RandomNumbers::Generator::Generator(const Config & cfg,
                                        unsigned int seed)
  : m_cfg{cfg}
  , m_engine{seed}
  , m_gauss{m_cfg.gauss_parameters[0],
            m_cfg.gauss_parameters[1]}
  , m_uniform{m_cfg.uniform_parameters[0],
              m_cfg.uniform_parameters[1]}
  , m_gamma{m_cfg.gamma_parameters[0],
            m_cfg.gamma_parameters[1]}
{
}

FW::RandomNumbers::Generator
FW::RandomNumbers::spawnGenerator(const AlgorithmContext & context) const
{
   const auto eventContext = context.eventContext;
   const unsigned int generatorID =
       context.algorithmNumber * eventContext->jobContext->eventCount
     + eventContext->eventNumber;

   return Generator{ m_cfg, m_cfg.seed + generatorID };
}

double
FW::RandomNumbers::Generator::draw(FW::Distribution dPar)
{
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
  }
  return 0.;
}

double
FW::RandomNumbers::draw(FW::Distribution dPar)
{
  return m_rng.draw(dPar);
}
