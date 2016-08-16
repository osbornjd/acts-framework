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
  , m_engine(0)
  , m_gauss(0., 1.)
  , m_uniform(0., 1.)
  , m_gamma(1., 1.)
{
  setConfiguration(cfg);
}

FW::ProcessCode
FW::RandomNumbers::setConfiguration(const FW::RandomNumbers::Config& cfg)
{
  m_cfg = cfg;

  // the engine
  m_engine = RandomEngine(m_cfg.seed);

  // the distributions
  m_gauss = GaussDist(m_cfg.gauss_parameters[0], m_cfg.gauss_parameters[1]);
  m_uniform
      = UniformDist(m_cfg.uniform_parameters[0], m_cfg.uniform_parameters[1]);
  m_gamma = GammaDist(m_cfg.gamma_parameters[0], m_cfg.gamma_parameters[1]);
  // return success
  return ProcessCode::SUCCESS;
}

double
FW::RandomNumbers::draw(FW::Distribution dType)
{
  switch (dType) {
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
