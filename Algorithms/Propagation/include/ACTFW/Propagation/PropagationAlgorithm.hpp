// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cmath>
#include <limits>
#include <memory>
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "Acts/EventData/NeutralParameters.hpp"
#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/Propagator/AbortList.hpp"
#include "Acts/Propagator/ActionList.hpp"
#include "Acts/Propagator/detail/DebugOutputActor.hpp"
#include "Acts/Propagator/detail/StandardAbortConditions.hpp"
#include "Acts/Propagator/detail/SteppingLogger.hpp"
#include "Acts/Surfaces/PerigeeSurface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Units.hpp"

/// @brief this test algorithm performs test propagation
/// within the Acts::Propagator
///
/// If the propagator is equipped appropriately, it can
/// also be used to test the Extrapolator within the geomtetry
///
/// @tparam propagator_t Type of the Propagator to be tested
namespace FW {

template <typename propagator_t>
class PropagationAlgorithm : public BareAlgorithm
{
public:
  struct Config
  {
    // create a config object with the propagator
    Config(propagator_t prop) : propagator(std::move(prop)) {}

    /// the propagors to be tested
    propagator_t propagator;

    /// how to set it up
    std::shared_ptr<RandomNumbersSvc> randomNumberSvc = nullptr;

    /// proapgation mode
    int mode = 0;
    /// debug output
    bool debugOutput = false;
    /// number of particles
    size_t ntests = 100;
    /// d0 gaussian sigma
    double d0Sigma = 15. * Acts::units::_um;
    /// z0 gaussian sigma
    double z0Sigma = 55. * Acts::units::_mm;
    /// phi range
    std::pair<double, double> phiRange = {-M_PI, M_PI};
    /// eta range
    std::pair<double, double> etaRange = {-4., 4.};
    /// pt range
    std::pair<double, double> ptRange
        = {100. * Acts::units::_MeV, 100. * Acts::units::_GeV};

    /// the step collection to be stored
    std::string propagationStepCollection = "PropagationSteps";

    /// covariance transport
    bool covarianceTransport = true;

    /// the covariance values
    Acts::ActsVectorD<5> covariances = Acts::ActsVectorD<5>::Zero();

    /// the correlation terms
    Acts::ActsSymMatrixD<5> correlations = Acts::ActsSymMatrixD<5>::Identity();
  };

  /// Constructor
  /// @param [in] cnf is the configuration struct
  /// @param [in] loglevel is the loggin level
  PropagationAlgorithm(const Config& cnf, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  FW::ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  Config m_cfg;  ///< the config class

  /// private helper method to create a corrleated covariance matrix
  /// @param[in] rnd is the random engine
  /// @param[in] gauss is a gaussian distribution to draw from
  std::unique_ptr<Acts::ActsSymMatrixD<5>>
  generateCovariance(FW::RandomEngine& rnd, FW::GaussDist& gauss) const;

  /// the templated execute test method for
  /// charged and netural particles
  /// @param [in] startParameters the start parameters
  template <typename parameters_t>
  std::vector<Acts::detail::Step>
  executeTest(const parameters_t& startParameters,
              double pathLength = std::numeric_limits<double>::max()) const
  {

    ACTS_DEBUG("Test extrapolation starts");

    // this is the outside in mode
    if (m_cfg.mode == 0) {

      // the step length logger for testing & end of world aborter
      typedef Acts::detail::SteppingLogger    SteppingLogger;
      typedef Acts::detail::EndOfWorldReached EndOfWorld;
      typedef Acts::detail::DebugOutputActor  DebugOutput;

      // Action list and abort list
      typedef Acts::ActionList<SteppingLogger, DebugOutput> ActionList;
      typedef Acts::AbortList<EndOfWorld> AbortConditions;

      // Create the propagation options
      typename propagator_t::template Options<ActionList, AbortConditions>
          options;
      options.pathLimit = pathLength;
      options.debug     = m_cfg.debugOutput;

      // Propagate using the propagator
      const auto& result = m_cfg.propagator.propagate(startParameters, options);
      auto steppingResults = result.template get<SteppingLogger::result_type>();

      if (m_cfg.debugOutput) {
        auto debugOutput = result.template get<DebugOutput::result_type>();
        ACTS_VERBOSE(debugOutput.debugString);
      }

      return steppingResults.steps;
    }
    return std::vector<Acts::detail::Step>();
  }
};

#include "PropagationAlgorithm.ipp"

}  // namespace FW
