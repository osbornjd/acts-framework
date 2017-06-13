#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Units.hpp"

FWA::ExtrapolationAlgorithm::ExtrapolationAlgorithm(
    const FWA::ExtrapolationAlgorithm::Config& cfg,
    std::unique_ptr<const Acts::Logger>        logger)
  : FW::Algorithm(cfg, std::move(logger)), m_cfg(cfg)
{
}

FWA::ExtrapolationAlgorithm::~ExtrapolationAlgorithm()
{
}

/** Framework finalize mehtod */
FW::ProcessCode
FWA::ExtrapolationAlgorithm::initialize(std::shared_ptr<FW::WhiteBoard> jStore)
{
  // call the algorithm initialize for setting the stores
  if (FW::Algorithm::initialize(jStore) != FW::ProcessCode::SUCCESS) {
    ACTS_FATAL("Algorithm::initialize() did not succeed!");
    return FW::ProcessCode::SUCCESS;
  }
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::ExtrapolationAlgorithm::execute(const FW::AlgorithmContext context) const
{
  // we read from a collection
  // -> will be outsourced into a simulation algorithm
  if (m_cfg.evgenParticlesCollection != "") {
    // Retrieve relevant information from the execution context
    auto eventStore = context.eventContext->eventStore;

    // prepare the input vector
    std::vector<Acts::ParticleProperties>* eventParticles = nullptr;
    // read and go
    if (eventStore
        && eventStore->readT(eventParticles, m_cfg.evgenParticlesCollection)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // run over it
    ACTS_INFO("Successfully read in collection with " << eventParticles->size()
                                                      << " particles");

    // create a new detector data hit container
    typedef std::pair<std::unique_ptr<const Acts::TrackParameters>,
                      barcode_type>
        FatrasHit;
    FW::DetectorData<geo_id_value, FatrasHit>* simulatedHits
        = m_cfg.simulatedHitsCollection != ""
        ? new FW::DetectorData<geo_id_value, FatrasHit>
        : nullptr;
    // prepare the output vector
    std::vector<Acts::ParticleProperties>* simulatedParticles
        = m_cfg.simulatedParticlesCollection != ""
        ? new std::vector<Acts::ParticleProperties>
        : nullptr;

    // counters
    size_t pCounter = 0;
    size_t sCounter = 0;
    for (auto& eParticle : (*eventParticles)) {
      // process the particle // TODO make configuraable
      if (eParticle.charge() != 0. && eParticle.vertex().perp() < 1.
          && eParticle.momentum().perp() > m_cfg.minPt
          && fabs(eParticle.momentum().eta()) < m_cfg.maxEta) {
        // count simulated particles
        ++pCounter;
        // record the simulated particles
        if (simulatedParticles) simulatedParticles->push_back(eParticle);
        // TODO update to better structure with Vertex-Particle tree
        Acts::PerigeeSurface surface(eParticle.vertex());
        double               d0    = 0.;
        double               z0    = 0.;
        double               phi   = eParticle.momentum().phi();
        double               theta = eParticle.momentum().theta();
        double qop = eParticle.charge() / eParticle.momentum().mag();
        // parameters
        Acts::ActsVectorD<5> pars;
        pars << d0, z0, phi, theta, qop;
        // some screen output
        std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
        // create the bound parameters
        Acts::BoundParameters startParameters(
            std::move(cov), std::move(pars), surface);
        if (executeTestT<Acts::TrackParameters>(startParameters,
                                                eParticle.barcode(),
                                                simulatedHits,
                                                m_cfg.ecChargedWriter)
            != FW::ProcessCode::SUCCESS)
          ACTS_VERBOSE("Test of parameter extrapolation did not succeed.");
      } else
        ++sCounter;
    }

    ACTS_INFO("Number of simulated particles : " << pCounter);
    ACTS_INFO("Number of skipped   particles : " << sCounter);
    if (eventStore && simulatedParticles
        && eventStore->writeT(simulatedParticles,
                              m_cfg.simulatedParticlesCollection)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING(
          "Could not write colleciton of simulated particles to event store.");
      return FW::ProcessCode::ABORT;
    } else if (simulatedParticles)
      ACTS_INFO("Truth information for "
                << simulatedParticles->size()
                << " particles written to EventStore.");

    // write to the EventStore
    if (eventStore && simulatedHits
        && eventStore->writeT(simulatedHits, m_cfg.simulatedHitsCollection)
            == FW::ProcessCode::ABORT) {
      ACTS_WARNING("Could not write colleciton of hits to event store.");
      return FW::ProcessCode::ABORT;
    } else if (simulatedHits)
      ACTS_INFO("Hit information for " << simulatedHits->size()
                                       << " volumes written to EventStore.");

  } else {
    // Create a random number generator
    FW::RandomNumbersSvc::Generator rng
        = m_cfg.randomNumbers->spawnGenerator(context);

    // loop
    for (size_t iex = 0; iex < m_cfg.testsPerEvent; ++iex) {
      // gaussian d0 and z0
      double d0 = m_cfg.d0Defs.at(0)
          + rng.drawGauss() * m_cfg.d0Defs.at(1);
      double z0 = m_cfg.z0Defs.at(0)
          + rng.drawGauss() * m_cfg.z0Defs.at(1);
      double phi = m_cfg.phiRange.at(0)
          + rng.drawUniform()
              * fabs(m_cfg.phiRange.at(1) - m_cfg.phiRange.at(0));
      double eta = m_cfg.etaRange.at(0)
          + rng.drawUniform()
              * fabs(m_cfg.etaRange.at(1) - m_cfg.etaRange.at(0));
      double theta = 2. * atan(exp(-eta));
      double pt    = m_cfg.ptRange.at(0)
          + rng.drawUniform()
              * fabs(m_cfg.ptRange.at(1) - m_cfg.ptRange.at(0));
      double p = pt / sin(theta);
      double q = rng.drawUniform() > 0.5 ? 1. : -1.;

      Acts::Vector3D momentum(
          p * sin(theta) * cos(phi), p * sin(theta) * sin(phi), p * cos(theta));
      std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
      Acts::ActsVectorD<5>                     pars;
      pars << d0, z0, phi, theta, q / p;
      // perigee parameters
      ACTS_VERBOSE("Building parameters from Perigee with (" << d0 << ", " << z0
                                                             << ", "
                                                             << phi
                                                             << ", "
                                                             << theta
                                                             << ", "
                                                             << q / p);
      // charged extrapolation
      Acts::PerigeeSurface pSurface(Acts::Vector3D(0., 0., 0.));

      // neutral extrapolation
      if (m_cfg.parameterType) {
        // prepare the start parameters
        Acts::BoundParameters startParameters(
            std::move(cov), std::move(pars), pSurface);
        if (executeTestT<Acts::TrackParameters>(
                startParameters, 0, nullptr, m_cfg.ecChargedWriter)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING(
              "Test of charged parameter extrapolation did not succeed.");

      } else {
        // charged extrapolation
        Acts::NeutralBoundParameters startParameters(
            std::move(cov), std::move(pars), pSurface);
        // prepare hits for charged neutral paramters
        if (executeTestT<Acts::NeutralParameters>(
                startParameters, 0, nullptr, m_cfg.ecNeutralWriter)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING(
              "Test of neutral parameter extrapolation did not succeed.");
      }
    }
  }
  // return SUCCESS to the frameword
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWA::ExtrapolationAlgorithm::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}
