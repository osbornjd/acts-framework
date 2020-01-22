// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include <memory>

#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/TruthTracking/VertexAndTracks.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"

using namespace Acts::UnitLiterals;

namespace FW {

/// @brief Converter that takes a generator event
/// (i.e. a std::vector<SimVertex>) and produces a
/// std::vector<Acts::BoundParameter>. It also allows
/// the possibility of parameter smearing.
class TruthVerticesToTracksAlgorithm : public BareAlgorithm
{
public:
  /// Nested configuration struct
  struct Config
  {
    /// Input particles
    std::string inputParticles = "";
    /// Input perigees
    std::string inputPerigees = "";
    /// Output track collection
    std::string output = "";
  };

  /// Constructor of the TruthVerticesToTracks algorithm
  ///
  /// @param cfg is the config struct to configure the algorihtm
  /// @param level is the logging level
  TruthVerticesToTracksAlgorithm(const Config&        cfg,
                                 Acts::Logging::Level level
                                 = Acts::Logging::INFO);

  /// Framework execute method of the digitization algorithm
  ///
  /// @param context is the algorithm context that holds event-wise information
  /// @return a process code to steer the algporithm flow
  ProcessCode
  execute(const AlgorithmContext& context) const final override;

private:
  /// Config struct
  Config m_cfg;

  /// @brief Function that corrects phi and theta wraps
  ///
  /// @param phiIn Phi
  /// @param thetaIn Theta
  void
  correctPhiThetaPeriodicity(double& phiIn, double& thetaIn) const;
};

}  // namespace FW
