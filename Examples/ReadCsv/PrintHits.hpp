// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstddef>
#include <string>

#include "ACTFW/Framework/BareAlgorithm.hpp"

namespace FW {

/// Select tracks by applying some selection cuts.
class PrintHits : public BareAlgorithm
{
public:
  struct Config
  {
    /// Input cluster collection.
    std::string inputClusters;
    /// Input hit-particles map.
    std::string inputHitParticlesMap;
    /// Input hit id collection
    std::string inputHitIds;
    /// Hit id range which should be logged.
    size_t hitIdStart  = 10224;
    size_t hitIdLength = 8;
    /// Detector selection for which input will be logged.
    size_t volumeId = 13;
    size_t layerId  = 4;
    size_t moduleId = 116;
  };

  PrintHits(const Config&        cfg,
            Acts::Logging::Level level = Acts::Logging::INFO);

  ProcessCode
  execute(const AlgorithmContext& ctx) const;

private:
  Config m_cfg;
};

}  // namespace FW
