//
//  RandomNumbersDistributions.cpp
//  ACTFW
//
//  Created by Hadrien Grasland on 27/06/17.
//
//

#include "ACTFW/Random/RandomNumberDistributions.hpp"


FW::LandauDist::param_type::param_type(double mean, double scale)
  : mean(mean)
  , scale(scale)
{
}

bool
FW::LandauDist::param_type::operator==(const param_type& other) const
{
    return (mean == other.mean) && (scale == other.scale);
}

FW::LandauDist::LandauDist(double mean, double scale)
  : m_cfg(mean, scale)
{
}

FW::LandauDist::LandauDist(const param_type& cfg)
  : m_cfg(cfg)
{
}

FW::LandauDist::result_type
FW::LandauDist::min() const
{
  return -std::numeric_limits<double>::infinity();
}

FW::LandauDist::result_type
FW::LandauDist::max() const
{
  return std::numeric_limits<double>::infinity();
}

bool
FW::LandauDist::operator==(const LandauDist& other) const
{
    return (m_cfg == other.m_cfg);
}
