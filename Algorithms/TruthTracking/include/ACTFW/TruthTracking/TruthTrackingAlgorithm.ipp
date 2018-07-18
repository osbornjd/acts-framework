// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

template <typename Propagator_type>
FW::TruthTracking<Propagator_type>::TruthTracking(const Config&        cfg,
                                                  Acts::Logging::Level loglevel)
  : FW::BareAlgorithm("TruthTracking", loglevel), m_cfg(cfg)
{
}

template <typename Propagator_type>
FW::ProcessCode
FW::TruthTracking<Propagator_type>::execute(
    const FW::AlgorithmContext context) const
{

  const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>*
      planarClusters
      = nullptr;

  // read and go
  if (context.eventStore.get(m_cfg.clusterCollection, planarClusters)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  // prepare the reverse truth / cluster map
  typedef std::pair<barcode_type, Acts::PlanarModuleCluster>     RtcPair;
  typedef std::multimap<barcode_type, Acts::PlanarModuleCluster> RtcMultiMap;
  typedef RtcMultiMap::iterator RtcIterator;
  RtcMultiMap                   rtcMap;

  for (auto& vData : (*planarClusters)) {
    auto volumeKey = vData.first;
    ACTS_DEBUG("- Processing Volume Data collection for volume with ID "
               << volumeKey);
    for (auto& lData : vData.second) {
      auto layerKey = lData.first;
      ACTS_DEBUG("-- Processing Layer Data collection for layer with ID "
                 << layerKey);
      for (auto& sData : lData.second) {
        auto moduleKey = sData.first;
        ACTS_DEBUG("-- Processing Module Data collection for module with ID "
                   << moduleKey);
        // get the hit parameters
        for (auto& cluster : sData.second) {
          // get all the associated particles
          for (auto& tvertex : cluster.truthVertices()) {
            auto& tposition = tvertex.position();
            for (auto& tparticle : tvertex.incomingParticles()) {
              // on entry per particle
              std::pair<barcode_type, Acts::PlanarModuleCluster> tc(
                  tparticle.barcode(), cluster);
              rtcMap.insert(tc);
            }
          }
        }
      }
    }
  }

  // Prepare the Propagator
  // Action list and abort list
  typedef Acts::Navigator                Navigator;
  typedef Acts::MaterialInteractor       Interactor;
  typedef Acts::detail::DebugOutputActor DebugOutput;

  typedef Acts::
      ActionList<Navigator, Interactor, JacobianCollector, DebugOutput>
          ActionList;

  typedef Acts::AbortList<> AbortList;
  // Propagator options for the reconstruction propagator
  typedef typename Propagator_type::template Options<ActionList, AbortList>
               Options_type;
  Options_type rOptions;

  // set some large setp size @ todo : make configurable
  rOptions.maxStepSize = 150. * Acts::units::_cm;
  rOptions.pathLimit   = 10. * Acts::units::_m;

  // get the fatras navigator and provide the TrackingGeometry
  auto& rNavigator = rOptions.action_list.template get<Acts::Navigator>();
  rNavigator.trackingGeometry = m_cfg.trackingGeometry;

  // create the extrapolator first
  typedef Extrapolator<Propagator_type, Options_type> Extrapolator;

  KalmanFitter<Extrapolator, CacheGenerator, NoCalibration, GainMatrixUpdator>
      kalmanFilter;

  kalmanFilter.m_oCacheGenerator = CacheGenerator();
  kalmanFilter.m_oCalibrator     = NoCalibration();
  kalmanFilter.m_oExtrapolator   = Extrapolator(m_cfg.propagator, rOptions);
  kalmanFilter.m_oUpdator        = GainMatrixUpdator();

  double   x  = 0.;
  double   y  = 0.;
  double   z  = 0.;
  double   px = 100.;
  double   py = 0.;
  double   pz = 0.;
  double   q  = 1;
  Vector3D pos(x, y, z);
  Vector3D mom(px, py, pz);

  // start covariance matrix
  auto startCov = std::make_unique<ActsSymMatrix<ParValue_t, NGlobalPars>>(
      ActsSymMatrix<ParValue_t, NGlobalPars>::Identity());
  (*startCov) = (*startCov) * 0.0001;

  auto startTP
      = std::make_unique<BoundParameters>(std::move(startCov),
                                          std::move(pos),
                                          std::move(mom),
                                          q,
                                          PerigeeSurface({0., 0., 0.}));

  // compare key lemma
  const auto compareKey = [](const RtcPair& lhs, const RtcPair& rhs) {
    return lhs.first < rhs.first;
  };

  // loop with compare opeator
  for (auto it = rtcMap.begin(); it != rtcMap.end();
       it      = std::upper_bound(it, rtcMap.end(), *it, compareKey)) {
    // get the equal range
    std::pair<RtcIterator, RtcIterator> rtcRange
        = rtcMap.equal_range(it->first);
    size_t simHits = std::distance(rtcRange.first, rtcRange.second);
    ACTS_VERBOSE("Found particle with  " << simHits);
    if (simHits >= m_cfg.minHits) {
      std::vector<PlanarModuleCluster> vMeasurements;
      for (auto itc = rtcRange.first; itc != rtcRange.second; ++itc)
        vMeasurements.push_back(itc->second);
      // sort the measurements
      sort(vMeasurements.begin(),
           vMeasurements.end(),
           [](const PlanarModuleCluster& lhs, const PlanarModuleCluster& rhs) {
             return lhs.referenceSurface().center().perp()
                 < rhs.referenceSurface().center().perp();
           });

      std::vector<FitMeas_t> fMeasurements;
      int                    id = 0;
      for (auto& cluster : vMeasurements) {

        auto                    parameters = cluster.parameters();
        double                  l1         = parameters[Acts::ParDef::eLOC_0];
        double                  l2         = parameters[Acts::ParDef::eLOC_1];
        double                  std1       = 0.01;
        double                  std2       = 0.01;
        Acts::ActsSymMatrixD<2> cov;
        cov << std1 * std1, 0, 0, std2 * std2;

        fMeasurements.push_back(Meas_t<eLOC_0, eLOC_1>(
            cluster.referenceSurface(), id, std::move(cov), l1, l2));
      }
      auto track = kalmanFilter.fit(fMeasurements, nullptr);
    }
  }

  /*
  // type the HitCollector
  typedef Fatras::Interactor<RandomEngine, SensitiveSelector> Interactor;

  // Action list and abort list
  typedef Acts::ActionList<Acts::Navigator, Interactor, DebugOutput>
    ActionList;


  typedef Acts::AbortList<> AbortList;
  // Propagator options
  typename Propagator_type::template Options<ActionList, AbortList>
    fOptions;


  // get the fatras interactor and set the samplers
  auto& fInteractor = fOptions.action_list.template get<Interactor>();
  // get and set the samplers
  fInteractor.multipleScatteringSampler  = m_cfg.multipleScatteringSampler;
  fInteractor.energyLossSampler          = m_cfg.energyLossSampler;
  fInteractor.energyLossSamplerElectrons = m_cfg.energyLossSamplerElectrons;
  fInteractor.hadronicInteractionSampler = m_cfg.hadronicInteractionSampler;
  // set the rest of the configuration
  fInteractor.debug                      = m_cfg.debugMode;
  //zfInteractor.minPt                      = m_cfg.minPt;
  //fInteractor.maxVr                      = m_cfg.maxVr;
  //fInteractor.maxVz                      = m_cfg.maxVz;
  //fInteractor.maxEta                     = m_cfg.maxEta;
  // set the per event random generator
  fInteractor.randomGenerator            = &rng;


  // read particles from input collection
  const std::vector<Acts::ProcessVertex>* evgen = nullptr;
  if (context.eventStore.get(m_cfg.evgenCollection, evgen)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  ACTS_DEBUG("read collection '" << m_cfg.evgenCollection << "' with "
                                 << evgen->size()
                                 << " vertices");

  // output: simulated particles attached to their process vertices
  std::vector<Acts::ProcessVertex> simulatedParticles;

  // output: hits - in detector data container
  FW::DetectorData<geo_id_value, FW::SimHit > simulatedHits;

  // loop over the vertices
  size_t evertices = 0;
  for (auto& evtx : (*evgen)) {
    ACTS_DEBUG("Processing event vertex no. " << evertices++);
    // vertex is outside cut
    if (evtx.position().perp() > m_cfg.maxVr) {
      ACTS_VERBOSE("Process vertex is outside the transverse cut. Skipping.");
      continue;
    }
    // the simulated particles associated to this vertex
    std::vector<Acts::ParticleProperties> sparticles;
    // the generated particles
    auto& gparticles = evtx.out();
    // copy simulated particles
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

    // create a new process vertex for the output collection
    Acts::ProcessVertex svertex(evtx.position(),
                                evtx.interactionTime(),
                                evtx.processType(),
                                {},
                                sparticles);
    simulatedParticles.push_back(svertex);

    // the asspcoated perigee for this vertex
    Acts::PerigeeSurface surface(evtx.position());

    // loop over particles
    for (const auto& particle : sparticles) {
      double d0    = 0.;
      double z0    = 0.;
      double phi   = particle.momentum().phi();
      double theta = particle.momentum().theta();
      double q     = particle.charge();
      // treat differently for neutral particles
      double qop = q != 0.
          ? q / particle.momentum().mag()
          : 1. / particle.momentum().mag();
      // parameters
      Acts::ActsVectorD<5> pars;
      pars << d0, z0, phi, theta, qop;
      // execute the test for charged particles
      if (q) {
        // charged extrapolation - with hit recording
        Acts::BoundParameters sParameters(nullptr, std::move(pars), surface);
        // forward material test
        const auto& fResult
            = m_cfg.propagator.propagate(sParameters, fOptions);
        auto& fInteractions = fResult.template get<Interactor::result_type>();
        ACTS_VERBOSE(" -> particle produced "
                      << fInteractions.sensitiveHits.size()
                      << " sensitive hits.");

        for (auto& fHit : fInteractions.sensitiveHits){
          if (fHit.surface){
            /// framework sim hit
            FW::SimHit fSimHit;
            fSimHit.surface   = fHit.surface;
            fSimHit.position  = fHit.position;
            fSimHit.momentum  = particle.momentum();
            fSimHit.value     = fHit.value;
            fSimHit.barcode   = particle.barcode();
            /// decode the geometry ID values
            geo_id_value volumeID
                = fSimHit.surface->geoID().value(Acts::GeometryID::volume_mask);
            geo_id_value layerID
              = fSimHit.surface->geoID().value(Acts::GeometryID::layer_mask);
            geo_id_value moduleID
              =
  fSimHit.surface->geoID().value(Acts::GeometryID::sensitive_mask);
            ACTS_VERBOSE(" --> hit produced on surface "
              <<  fSimHit.surface->geoID().toString());
            /// insert the simulate dhit
            FW::Data::insert(simulatedHits,
                             volumeID,
                             layerID,
                             moduleID,
                             std::move(fSimHit));
          }
        }
      }
    }
  }

  // write simulated data to the event store
  // - the particles
  if (context.eventStore.add(m_cfg.simulatedParticleCollection,
                         std::move(simulatedParticles))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // - the hits
  if (context.eventStore.add(m_cfg.simulatedHitCollection,
                         std::move(simulatedHits))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  */
  return FW::ProcessCode::SUCCESS;
}
