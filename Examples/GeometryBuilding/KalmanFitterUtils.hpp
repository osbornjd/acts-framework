// This file is part of the ACTS project.
//
// Copyright (C) 2017-2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTS_KALMANFITUTILS_H
#define ACTS_KALMANFITUTILS_H

#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <vector>
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/EventData/Measurement.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Extrapolation/ExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Extrapolation/MaterialEffectsEngine.hpp"
#include "ACTS/Extrapolation/RungeKuttaEngine.hpp"
#include "ACTS/Extrapolation/StaticEngine.hpp"
#include "ACTS/Extrapolation/StaticNavigationEngine.hpp"
#include "ACTS/Fitter/KalmanFitter.hpp"
#include "ACTS/Fitter/KalmanUpdator.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Utilities/Units.hpp"


namespace Acts {

/// Contents: `FaserCache`, `FaserExtrapolator`, `FaserCache`, `FaserCacheGenerator`
///           `initExtrapolator`


typedef Acts::FittableMeasurement<long int> FitMeas_t;
template <ParID_t... pars>
using Meas_t = Acts::Measurement<long int, pars...>;


struct FaserCache
{
  std::unique_ptr<const KF::Step<long int>::JacobianMatrix> jacobian;
  std::unique_ptr<const BoundParameters>                    parameters;

  FaserCache()                  = default;
  FaserCache(const FaserCache&) = delete;
  FaserCache(FaserCache&&)      = default;
};


class FaserExtrapolator
{
public:
  FaserExtrapolator(std::shared_ptr<const IExtrapolationEngine> exEngine
                 = nullptr);

  FaserCache
  operator()(const FitMeas_t& m, const TrackParameters& tp) const;

private:
  std::shared_ptr<const IExtrapolationEngine> m_exEngine;
};


class FaserCalibrator
{
public:
  std::unique_ptr<const FitMeas_t>
  operator()(const FitMeas_t& m, const BoundParameters&) const;
};


class FaserCacheGenerator
{
public:
  std::unique_ptr<KF::Step<long int>>
  operator()(FaserCache m) const;
};


std::shared_ptr<IExtrapolationEngine>
initExtrapolator(const std::shared_ptr<const TrackingGeometry>& geo);


////////////////////////////////////////////////////////////////////////////////
// TODO: Move the following out of the header file.
////////////////////////////////////////////////////////////////////////////////

FaserExtrapolator::FaserExtrapolator(std::shared_ptr<const IExtrapolationEngine> exEngine)
  : m_exEngine(std::move(exEngine))
{
};


FaserCache
FaserExtrapolator::operator()(const FitMeas_t& m, const TrackParameters& tp) const
{
  auto exCell = std::make_unique<ExtrapolationCell<TrackParameters>>(tp);
  exCell->addConfigurationMode(ExtrapolationMode::CollectJacobians);
  (*exCell).pathLimit = 500;
  const Surface& sf   = getSurface(m);

  m_exEngine->extrapolate(*exCell, &sf);
  FaserCache c;
  auto    j = exCell->extrapolationSteps.back().transportJacobian.release();
  c.jacobian.reset(new KF::Step<long int>::JacobianMatrix(*j));
  auto pars
      = static_cast<const BoundParameters*>(exCell->leadParameters->clone());
  c.parameters.reset(pars);

  return c;
};


std::unique_ptr<const FitMeas_t>
Acts::FaserCalibrator::operator()(const FitMeas_t& m, const BoundParameters&) const
{
  return std::make_unique<const FitMeas_t>(m);
};

std::unique_ptr<KF::Step<long int>>
FaserCacheGenerator::operator()(FaserCache m) const
{
  auto step = std::make_unique<KF::Step<long int>>();
  step->setPredictedState(std::move(m.parameters));
  step->setJacobian(std::move(m.jacobian));

  return step;
};


std::shared_ptr<IExtrapolationEngine>
initExtrapolator(const std::shared_ptr<const TrackingGeometry>& geo)
{
  auto propConfig = RungeKuttaEngine<>::Config();
  propConfig.fieldService
      = std::make_shared<ConstantBField>(0, 0, 2 * Acts::units::_T);
  auto propEngine = std::make_shared<RungeKuttaEngine<>>(propConfig);

  auto matConfig      = MaterialEffectsEngine::Config();
  auto materialEngine = std::make_shared<MaterialEffectsEngine>(matConfig);

  auto navConfig                  = StaticNavigationEngine::Config();
  navConfig.propagationEngine     = propEngine;
  navConfig.materialEffectsEngine = materialEngine;
  navConfig.trackingGeometry      = geo;
  auto navEngine = std::make_shared<StaticNavigationEngine>(navConfig);

  auto statConfig                  = StaticEngine::Config();
  statConfig.propagationEngine     = propEngine;
  statConfig.navigationEngine      = navEngine;
  statConfig.materialEffectsEngine = materialEngine;
  auto statEngine                  = std::make_shared<StaticEngine>(statConfig);

  auto exEngineConfig                 = ExtrapolationEngine::Config();
  exEngineConfig.trackingGeometry     = geo;
  exEngineConfig.propagationEngine    = propEngine;
  exEngineConfig.navigationEngine     = navEngine;
  exEngineConfig.extrapolationEngines = {statEngine};
  auto exEngine = std::make_shared<ExtrapolationEngine>(exEngineConfig);
  exEngine->setLogger(getDefaultLogger("ExtrapolationEngine", Logging::INFO));

  return exEngine;
};

} // end namespace Acts

#endif
