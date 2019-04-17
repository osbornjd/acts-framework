// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <random>

#include <Acts/Utilities/Helpers.hpp>

template <typename propagator_t>
std::unique_ptr<Acts::ActsSymMatrixD<5>>
PropagationAlgorithm<propagator_t>::generateCovariance(
    FW::RandomEngine&                 rnd,
    std::normal_distribution<double>& gauss) const
{
  if (m_cfg.covarianceTransport && m_cfg.randomNumbers) {
    // we start from the correlation matrix
    auto newCov = std::make_unique<Acts::ActsSymMatrixD<5>>(m_cfg.correlations);
    // then we draw errors according to the error values
    Acts::ActsVectorD<5> covs_smeared = m_cfg.covariances;
    for (size_t k = 0; k < 5; ++k) covs_smeared[k] *= gauss(rnd);
    // and apply a double loop
    for (size_t i = 0; i < 5; ++i)
      for (size_t j = 0; j < 5; ++j) {
        (*newCov)(i, j) *= covs_smeared[i];
        (*newCov)(i, j) *= covs_smeared[j];
      }
    return std::move(newCov);
  }
  return nullptr;
}

template <typename propagator_t>
PropagationAlgorithm<propagator_t>::PropagationAlgorithm(
    const PropagationAlgorithm<propagator_t>::Config& cfg,
    Acts::Logging::Level                              loglevel)
  : BareAlgorithm("PropagationAlgorithm", loglevel), m_cfg(cfg)
{
}

/// Templated execute test method for
/// charged and netural particles
/// @param [in] context is the contextual data of this event
/// @param [in] startParameters the start parameters
/// @param [in] pathLength the maximal path length to go
template <typename propagator_t>
template <typename parameters_t>
PropagationOutput
PropagationAlgorithm<propagator_t>::executeTest(
    const AlgorithmContext& context,
    const parameters_t&     startParameters,
    double                  pathLength) const
{

  ACTS_DEBUG("Test propagation/extrapolation starts");

  PropagationOutput pOutput;

  // This is the outside in mode
  if (m_cfg.mode == 0) {

    // The step length logger for testing & end of world aborter
    using MaterialInteractor = Acts::MaterialInteractor;
    using SteppingLogger     = Acts::detail::SteppingLogger;
    using DebugOutput        = Acts::detail::DebugOutputActor;
    using EndOfWorld         = Acts::detail::EndOfWorldReached;

    // Action list and abort list
    using ActionList
        = Acts::ActionList<SteppingLogger, MaterialInteractor, DebugOutput>;
    using AbortList         = Acts::AbortList<EndOfWorld>;
    using PropagatorOptions = Acts::PropagatorOptions<ActionList, AbortList>;

    PropagatorOptions options(context.geoContext, context.magFieldContext);
    options.pathLimit = pathLength;
    options.debug     = m_cfg.debugOutput;

    // Activate loop protection at some pt value
    options.loopProtection
        = (Acts::VectorHelpers::perp(startParameters.momentum())
           < m_cfg.ptLoopers);

    // Switch the material interaction on/off & eventually into logging mode
    auto& mInteractor = options.actionList.get<MaterialInteractor>();
    mInteractor.multipleScattering = m_cfg.multipleScattering;
    mInteractor.energyLoss         = m_cfg.energyLoss;
    mInteractor.recordInteractions = m_cfg.recordMaterialInteractions;

    // Set a maximum step size
    options.maxStepSize = m_cfg.maxStepSize;

    // Propagate using the propagator
    const auto& result
        = m_cfg.propagator.propagate(startParameters, options).value();
    auto steppingResults = result.template get<SteppingLogger::result_type>();

    // Set the stepping result
    pOutput.first = std::move(steppingResults.steps);
    // Also set the material recording result - if configured
    if (m_cfg.recordMaterialInteractions) {
      auto materialResult
          = result.template get<MaterialInteractor::result_type>();
      pOutput.second = std::move(materialResult);
    }
  }
  return pOutput;
}

template <typename propagator_t>
ProcessCode
PropagationAlgorithm<propagator_t>::execute(
    const AlgorithmContext& context) const
{
  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumberSvc->spawnGenerator(context);

  // Setup random number distributions for some quantities
  std::normal_distribution<double>       d0Dist(0., m_cfg.d0Sigma);
  std::normal_distribution<double>       z0Dist(0., m_cfg.z0Sigma);
  std::uniform_real_distribution<double> phiDist(m_cfg.phiRange.first,
                                                 m_cfg.phiRange.second);
  std::uniform_real_distribution<double> etaDist(m_cfg.etaRange.first,
                                                 m_cfg.etaRange.second);
  std::uniform_real_distribution<double> ptDist(m_cfg.ptRange.first,
                                                m_cfg.ptRange.second);
  std::uniform_real_distribution<double> qDist(0., 1.);

  std::shared_ptr<const Acts::PerigeeSurface> surface
      = Acts::Surface::makeShared<Acts::PerigeeSurface>(
          Acts::Vector3D(0., 0., 0.));

  // Output : the propagation steps
  std::vector<std::vector<Acts::detail::Step>> propagationSteps;
  propagationSteps.reserve(m_cfg.ntests);

  // Output (optional): the recorded material
  std::vector<RecordedMaterialTrack> recordedMaterial;
  if (m_cfg.recordMaterialInteractions) {
    recordedMaterial.reserve(m_cfg.ntests);
  }

  // loop over number of particles
  for (size_t it = 0; it < m_cfg.ntests; ++it) {
    /// get the d0 and z0
    double d0     = d0Dist(rng);
    double z0     = z0Dist(rng);
    double phi    = phiDist(rng);
    double eta    = etaDist(rng);
    double theta  = 2 * atan(exp(-eta));
    double pt     = ptDist(rng);
    double p      = pt / sin(theta);
    double charge = qDist(rng) > 0.5 ? 1. : -1.;
    double qop    = charge / p;
    // parameters
    Acts::ActsVectorD<5> pars;
    pars << d0, z0, phi, theta, qop;
    // some screen output
    std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;

    Acts::Vector3D sPosition(0., 0., 0.);
    Acts::Vector3D sMomentum(0., 0., 0.);

    // execute the test for charged particles
    PropagationOutput pOutput;
    if (charge) {
      // charged extrapolation - with hit recording
      Acts::BoundParameters startParameters(
          context.geoContext, std::move(cov), std::move(pars), surface);
      sPosition = startParameters.position();
      sMomentum = startParameters.momentum();
      pOutput   = executeTest<Acts::TrackParameters>(context, startParameters);
    } else {
      // execute the test for neeutral particles
      Acts::NeutralBoundParameters neutralParameters(
          context.geoContext, std::move(cov), std::move(pars), surface);
      sPosition = neutralParameters.position();
      sMomentum = neutralParameters.momentum();
      pOutput
          = executeTest<Acts::NeutralParameters>(context, neutralParameters);
    }
    // Record the propagator steps
    propagationSteps.push_back(std::move(pOutput.first));
    if (m_cfg.recordMaterialInteractions
        && pOutput.second.materialInteractions.size()) {
      // Create a recorded material track
      RecordedMaterialTrack rmTrack;
      // Start position
      rmTrack.first.first = std::move(sPosition);
      // Start momentum
      rmTrack.first.second = std::move(sMomentum);
      // The material
      rmTrack.second = std::move(pOutput.second);
      // push it it
      recordedMaterial.push_back(std::move(rmTrack));
    }
  }

  // Write the propagation step data to the event store
  if (context.eventStore.add(m_cfg.propagationStepCollection,
                             std::move(propagationSteps))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  // Write the recorded material to the event store
  if (m_cfg.recordMaterialInteractions
      && context.eventStore.add(m_cfg.propagationMaterialCollection,
                                std::move(recordedMaterial))
          == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  return ProcessCode::SUCCESS;
}
