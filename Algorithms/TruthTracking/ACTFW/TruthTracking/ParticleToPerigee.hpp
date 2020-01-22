// This file is part of the Acts project.
//
// Copyright (C) 2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "Acts/Utilities/Definitions.hpp"

namespace FW {

/// Propagates true particle tracks to the perigee
/// and determines the parameters at the perigee
class ParticleToPerigee final : public BareAlgorithm
{
public:
  struct Config
  {
    /// The input truth particles that should be used to create proto tracks.
    std::string inputParameters;

    /// The output proto tracks collection.
    std::string outputPerigee;

    /// The magnetic field
    Acts::Vector3D bField;

    /// Reference position
    Acts::Vector3D refPosition = Acts::Vector3D::Zero();
  };

  ParticleToPerigee(const Config& cfg, Acts::Logging::Level lvl);

  ProcessCode
  execute(const AlgorithmContext& ctx) const override final;

private:
  Config m_cfg;
};

}  // namespace FW
