// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @date 2018-03-14
/// @author Moritz Kiehn <msmk@cern.ch>

#pragma once

#include <limits>

#include "ACTFW/Framework/BareAlgorithm.hpp"

namespace FW {

/// Select particles by applying some selection cuts.
class ParticleSelector : public BareAlgorithm
{
public:
  struct Config
  {
    /// The input event collection.
    std::string inputEvent;
    /// The output event collection.
    std::string outputEvent;
    /// Minimum distance from the origin in the tranverse plane.
    double rhoMin = 0;
    /// Maximum distance from the origin in the transverse plane.
    double rhoMax = std::numeric_limits<double>::max();
    /// Maximum absolute distance from the origin along z.
    double absZMax = std::numeric_limits<double>::max();
    // Particle cuts.
    double phiMin    = std::numeric_limits<double>::lowest();
    double phiMax    = std::numeric_limits<double>::max();
    double etaMin    = std::numeric_limits<double>::lowest();
    double etaMax    = std::numeric_limits<double>::max();
    double absEtaMin = std::numeric_limits<double>::lowest();
    double absEtaMax = std::numeric_limits<double>::max();
    double ptMin     = 0.0;
    double ptMax     = std::numeric_limits<double>::max();
    /// Keep neutral particles.
    bool keepNeutral = true;
  };

  ParticleSelector(const Config& cfg, Acts::Logging::Level lvl);

  ProcessCode
  execute(const AlgorithmContext& ctx) const;

private:
  Config m_cfg;
};

}  // namespace FW
