// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimHit.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolator/Navigator.hpp"
#include "ACTS/Propagator/AbortList.hpp"
#include "ACTS/Propagator/ActionList.hpp"
#include "ACTS/Propagator/detail/DebugOutputActor.hpp"
#include "Fatras/EnergyLoss.hpp"
#include "Fatras/Interactor.hpp"
#include "Fatras/detail/Definitions.hpp"

template <typename Propagator_type>
FW::FatrasAlgorithm<Propagator_type>::FatrasAlgorithm(
    const Config&        cfg,
    Acts::Logging::Level loglevel)
  : FW::BareAlgorithm("FatrasAlgorithm", loglevel), m_cfg(cfg)
{
}

template <typename Propagator_type>
FW::ProcessCode
FW::FatrasAlgorithm<Propagator_type>::execute(
    const FW::AlgorithmContext context) const
{

  // Create an algorithm local random number generator
  RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(context);

  // type the HitCollector
  typedef Fatras::Interactor<RandomEngine, SensitiveSelector> Interactor;
  typedef Acts::detail::DebugOutputActor DebugOutput;

  // Action list and abort list
  typedef Acts::ActionList<Acts::Navigator, Interactor, DebugOutput> ActionList;

  typedef Acts::AbortList<> AbortList;
  // Propagator options
  typename Propagator_type::template Options<ActionList, AbortList> fOptions;

  // set some large setp size @ todo : make configurable
  fOptions.maxStepSize = 150. * Acts::units::_cm;
  fOptions.pathLimit   = 10. * Acts::units::_m;
  fOptions.debug       = m_cfg.debugMode;

  // get the fatras navigator and provide the TrackingGeometry
  auto& fNavigator = fOptions.action_list.template get<Acts::Navigator>();
  fNavigator.trackingGeometry = m_cfg.trackingGeometry;
  fNavigator.debug            = m_cfg.debugMode;

  // get the fatras interactor and set the samplers
  auto& fInteractor = fOptions.action_list.template get<Interactor>();
  // get and set the samplers
  fInteractor.multipleScatteringSampler  = m_cfg.multipleScatteringSampler;
  fInteractor.energyLossSampler          = m_cfg.energyLossSampler;
  fInteractor.energyLossSamplerElectrons = m_cfg.energyLossSamplerElectrons;
  fInteractor.hadronicInteractionSampler = m_cfg.hadronicInteractionSampler;
  // set the rest of the configuration
  fInteractor.debug = m_cfg.debugMode;
  // zfInteractor.minPt                      = m_cfg.minPt;
  // fInteractor.maxVr                      = m_cfg.maxVr;
  // fInteractor.maxVz                      = m_cfg.maxVz;
  // fInteractor.maxEta                     = m_cfg.maxEta;
  // set the per event random generator
  fInteractor.randomGenerator = &rng;

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
  FW::DetectorData<geo_id_value, FW::SimHit> simulatedHits;

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
    auto& gparticles = evtx.outgoingParticles();
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
      double qop = q != 0. ? q / particle.momentum().mag()
                           : 1. / particle.momentum().mag();
      // parameters
      Acts::ActsVectorD<5> pars;
      pars << d0, z0, phi, theta, qop;
      // execute the test for charged particles
      if (q) {
        // charged extrapolation - with hit recording
        Acts::BoundParameters sParameters(nullptr, std::move(pars), surface);
        // forward material test
        const auto& fResult = m_cfg.propagator.propagate(sParameters, fOptions);
        auto& fInteractions = fResult.template get<Interactor::result_type>();
        ACTS_VERBOSE(" -> particle produced "
                     << fInteractions.sensitiveHits.size()
                     << " sensitive hits.");

        for (auto& fHit : fInteractions.sensitiveHits) {
          if (fHit.surface) {
            /// framework sim hit
            FW::SimHit fSimHit;
            fSimHit.surface  = fHit.surface;
            fSimHit.position = fHit.position;
            fSimHit.momentum = particle.momentum();
            fSimHit.value    = fHit.value;
            fSimHit.barcode  = particle.barcode();
            /// decode the geometry ID values
            geo_id_value volumeID
                = fSimHit.surface->geoID().value(Acts::GeometryID::volume_mask);
            geo_id_value layerID
                = fSimHit.surface->geoID().value(Acts::GeometryID::layer_mask);
            geo_id_value moduleID = fSimHit.surface->geoID().value(
                Acts::GeometryID::sensitive_mask);
            ACTS_VERBOSE(" --> hit produced on surface "
                         << fSimHit.surface->geoID().toString());
            /// insert the simulate dhit
            FW::Data::insert(
                simulatedHits, volumeID, layerID, moduleID, std::move(fSimHit));
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
  return FW::ProcessCode::SUCCESS;
}
