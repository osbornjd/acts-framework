// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include <memory>
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "Acts/Utilities/Units.hpp"

namespace FW {

class EventToTrackConverterAlgorithm : public BareAlgorithm
{
public:
  /// Nested configuration struct
  struct Config
  {
    /// Input event (vertex collection)
    std::string inputCollection = "";
    /// Output track collection
    std::string outputCollection = "";

    /// FW random number service
    std::shared_ptr<RandomNumbersSvc> randomNumberSvc = nullptr;

    /// The magnetic field
    Acts::ConstantBField bField;

    /// Reference position relatice to which track
    /// parameters will be defined
    Acts::Vector3D refPosition = Acts::Vector3D::Zero();

    /// Do track smearing
    bool doSmearing = true;

    /// Define parameter for pt-dependent IP resolution
    /// of the form sigma_d/z(p_t[GeV]) = A*exp(-B*p_t[GeV]) + C
    double ipResA = 100.7439 * Acts::units::_um;
    double ipResB = 0.23055;
    double ipResC = 20. * Acts::units::_um;
  };

  /// Constructor of the EventToTrackConverter algorithm
  ///
  /// @param cfg is the config struct to configure the algorihtm
  /// @param level is the logging level
  EventToTrackConverterAlgorithm(const Config&        cfg,
                                 Acts::Logging::Level level
                                 = Acts::Logging::INFO);

  /// Framework execute method of the digitization algorithm
  ///
  /// @param context is the algorithm context that holds event-wise information
  /// @return a process code to steer the algporithm flow
  ProcessCode
  execute(const AlgorithmContext& context) const final override;

private:
  Config m_cfg;  /// config struct

  void
  correctPhiThetaPeriodicity(double& phiIn, double& thetaIn) const;
};

}  // namespace FW
