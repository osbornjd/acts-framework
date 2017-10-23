// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

template <typename PropagatorA, typename PropagatorB, typename PropagatorE>
std::unique_ptr<Acts::ActsSymMatrixD<5>>
PropagationAlgorithm<PropagatorA, PropagatorB, PropagatorE>::generateCovariance(
    FW::RandomEngine& rnd,
    FW::GaussDist&    gauss) const
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

template <typename PropagatorA, typename PropagatorB, typename PropagatorE>
PropagationAlgorithm<PropagatorA, PropagatorB, PropagatorE>::
    PropagationAlgorithm(const PropagationAlgorithm<PropagatorA,
                                                    PropagatorB,
                                                    PropagatorE>::Config& cfg,
                         Acts::Logging::Level loglevel)
  : BareAlgorithm("PropagationAlgorithm", loglevel), m_cfg(cfg)
{
  m_optionsA.max_path_length = m_cfg.pathLimit;
  m_optionsB.max_path_length = m_cfg.pathLimit;
  // create the surface
  m_surface = std::make_unique<Acts::CylinderSurface>(
      nullptr, 2. * m_cfg.pathLimit, std::numeric_limits<double>::max());
  // create the radial surfaces
  for (auto& r : m_cfg.cylinderRadii) {
    m_radialSurfaces.push_back(std::make_unique<Acts::CylinderSurface>(
        nullptr, r, std::numeric_limits<double>::max()));
  }
}

template <typename PropagatorA, typename PropagatorB, typename PropagatorE>
ProcessCode
PropagationAlgorithm<PropagatorA, PropagatorB, PropagatorE>::execute(
    AlgorithmContext ctx) const
{

  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber);
  ACTS_VERBOSE("Test  mode configuration is: " << m_cfg.testMode);

  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);
  // Spawn some random number distributions
  FW::GaussDist gauss(0., 1.);

  // read particles from input collection
  const std::vector<Acts::ProcessVertex>* evgen = nullptr;
  if (ctx.eventStore.get(m_cfg.evgenCollection, evgen) == ProcessCode::ABORT)
    return ProcessCode::ABORT;

  ACTS_DEBUG("read collection '" << m_cfg.evgenCollection << "' with "
                                 << evgen->size()
                                 << " vertices");

  // loop over the vertices
  size_t evertices = 0;
  for (auto& evtx : (*evgen)) {
    ACTS_DEBUG("Processing event vertex no. " << evertices++);
    // vertex is outside cut
    if (evtx.position().perp() > m_cfg.maxD0) {
      ACTS_VERBOSE("Process vertex is outside the transverse cut. Skipping.");
      continue;
    }
    // the simulated particles associated to this vertex
    std::vector<Acts::ParticleProperties> sparticles;
    // the generated particles
    auto& gparticles = evtx.outgoingParticles();

    std::copy_if(gparticles.begin(),
                 gparticles.end(),
                 std::back_inserter(sparticles),
                 [=](const auto& particle) {
                   return (std::abs(particle.momentum().eta()) < m_cfg.maxEta)
                       && (m_cfg.minPt < particle.momentum().perp());
                 });
    ACTS_DEBUG("Skipped   particles: " << gparticles.size()
                   - sparticles.size());
    ACTS_DEBUG("Simulated particles: " << sparticles.size());

    // the asspcoated perigee for this vertex
    const auto& vertex = evtx.position();

    // prepare the output collection
    std::vector<std::vector<tp_ptr>> tParametersCollection;
    tParametersCollection.reserve(sparticles.size());

    // loop over particles and run the test
    for (const auto& particle : sparticles) {
      // create the output collection
      std::vector<tp_ptr> tParameters;
      // this is the momentum
      const auto& momentum = particle.momentum();
      double      charge   = particle.charge();
      // execute the test for charged particles
      if (particle.charge()) {

        // some screen output
        std::unique_ptr<Acts::ActsSymMatrixD<5>> cov
            = generateCovariance(rng, gauss);
        Acts::CurvilinearParameters sParameters(
            std::move(cov), vertex, momentum, charge);
        // record the start paramters
        auto sPars = sParameters.clone();
        tParameters.push_back(std::move(tp_ptr(sPars)));
        // the path length test
        if (m_cfg.testMode == pathLength) {
          ACTS_VERBOSE("Testing path length propagation ...");
          // the first propagation
          if (m_cfg.propagatorA)
            propagateAB(
                m_cfg.propagatorA, sParameters, m_optionsA, tParameters);
          // the second propagation
          if (m_cfg.propagatorB)
            propagateAB(
                m_cfg.propagatorB, sParameters, m_optionsB, tParameters);
          // the IPropagatorEngine - needs surface due to old design
          auto sf = m_surface.get();
          if (m_cfg.propagatorE)
            propagateE(m_cfg.propagatorE, sParameters, *sf, tParameters);
        }

        // the kalman filter test
        if (m_cfg.testMode == kalman) {
          ACTS_VERBOSE("Testing kalman filter like propagation ...");
          // the first propagtor to be tested
          if (m_cfg.propagatorA) {
            // last surface for cross-check
            const Acts::Surface* lSurface = NULL;
            // initial parameters
            const Acts::TrackParameters* parameters = &(sParameters);
            // cache creation
            typename PropagatorA::cache_type cacheA(sParameters);
            size_t                           npars = tParameters.size();
            // loop over surfaces
            for (auto& surface : m_radialSurfaces) {
              if (m_cfg.cacheCall)
                propagateCacheAB(
                    m_cfg.propagatorA,
                    *parameters,
                    *(surface.get()),
                    cacheA,
                    m_optionsA,
                    tParameters) else propagateSfAB(m_cfg.propagatorA,
                                                    *parameters,
                                                    *(surface.get()),
                                                    m_optionsA,
                                                    tParameters);
              // indicate success  ful propagation
              size_t cpars = tParameters.size();
              if (npars != cpars) {
                lSurface   = surface.get();
                parameters = tParameters[cpars - 1].get();
                npars      = cpars;
              }
            }
            // start to end propagation
            if (lSurface)
              propagateSfAB(m_cfg.propagatorA,
                            sParameters,
                            *lSurface,
                            m_optionsA,
                            tParameters);
          }
          //// the second propagtor to be tested
          // if (m_cfg.propagatorB){
          //  // initial parameters
          //  const Acts::TrackParameters* parameters = &(sParameters);
          //  // cache creation
          //  typename Propagator::Cache cacheB(sParameters)
          //  size_t npars = tParameters.size();
          //  // loop over surfaces
          //  for (auto& surface : m_radialSurfaces){
          //    if (m_cfg.cacheCall) propagateCacheAB(m_cfg.propagatorB,
          //                                           *parameters,
          //                                           surface.get(),
          //                                           cacheB,
          //                                           m_optionsB,
          //                                           tParameters);
          //    else propagateAB((m_cfg.propagatorB,
          //                      *parameters,
          //                      surface.get(),
          //                      m_optionsB,
          //                      tParameters);
          //   // indicate successful propagation
          //   size_t cpars = tParameters.size();
          //   if (npars != cpars){
          //     lSurface = surface.get();
          //     parameters = tParameters[cpars-1].get();
          //     npars = cpars;
          //   }
          //  }
          //  // start to end propagation
          //  propagateCacheAB(m_cfg.propagatorB,
          //                   sParameters,
          //                   lSurface,
          //                   cacheB,
          //                   m_optionsB,
          //                   tParameters);
          //}

          // test the same thing with the PropagatorE
          if (m_cfg.propagatorE) {
            // last surface for cross-check
            const Acts::Surface* lSurface = NULL;
            // initial parameters
            const Acts::TrackParameters* parameters = &(sParameters);
            size_t                       npars      = tParameters.size();
            // loop over surfaces
            for (auto& surface : m_radialSurfaces) {
              propagateE(m_cfg.propagatorE,
                         *parameters,
                         *(surface.get()),
                         tParameters);
              // indicate success  ful propagation
              size_t cpars = tParameters.size();
              if (npars != cpars) {
                lSurface   = surface.get();
                parameters = tParameters[cpars - 1].get();
                npars      = cpars;
              }
            }
            // start to end propagation
            if (lSurface)
              propagateE(
                  m_cfg.propagatorE, *parameters, *lSurface, tParameters);
          }
        }
      }  // charged particle
      // write out collection
      tParametersCollection.push_back(std::move(tParameters));
    }  // loop over particles per vertex

    // - the extrapolation cells - charged - if configured
    if (m_cfg.trackParametersCollection != ""
        && ctx.eventStore.add(m_cfg.trackParametersCollection,
                              std::move(tParametersCollection))
            == ProcessCode::ABORT) {
      return ProcessCode::ABORT;
    }
  }  // loop over event vertices
  return ProcessCode::SUCCESS;
}
