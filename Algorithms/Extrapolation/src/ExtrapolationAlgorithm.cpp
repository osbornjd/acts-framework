#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"

#include <iostream>
#include <random>

#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Extrapolation/IExtrapolationEngine.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Units.hpp"

FW::ExtrapolationAlgorithm::ExtrapolationAlgorithm(
    const FW::ExtrapolationAlgorithm::Config& cfg)
  : FW::BareAlgorithm("ExtrapolationAlgorithm"), m_cfg(cfg)
{
}

FW::ProcessCode
FW::ExtrapolationAlgorithm::execute(FW::AlgorithmContext ctx) const
{
  // read particles from input collection
  const std::vector<Acts::ParticleProperties>* particles = nullptr;
  if (ctx.eventStore.get(m_cfg.particlesCollection, particles)
      == FW::ProcessCode::ABORT)
    return FW::ProcessCode::ABORT;

  ACTS_DEBUG("read collection '" << m_cfg.particlesCollection << "' with "
                                 << particles->size()
                                 << " particles");

  // selected particles that should be simulated
  std::vector<Acts::ParticleProperties> simulated;
  std::copy_if(particles->begin(),
               particles->end(),
               std::back_inserter(simulated),
               [=](const auto& particle) {
                 return (particle.charge() != 0. || !m_cfg.skipNeutral)
                     && (particle.vertex().perp() < m_cfg.maxD0)
                     && (std::abs(particle.momentum().eta()) < m_cfg.maxEta)
                     && (m_cfg.minPt < particle.momentum().perp());
               });
  ACTS_DEBUG("Skipped   particles: " << particles->size() - simulated.size());
  ACTS_DEBUG("Simulated particles: " << simulated.size());

  // propagate particles through the detector and generate hits
  using FatrasHit
      = std::pair<std::unique_ptr<const Acts::TrackParameters>, barcode_type>;

  // prepare the detector data dfoe qeirinf out
  FW::DetectorData<geo_id_value, FatrasHit> hits;
  
  // the charged and neutral containers of the particles
  std::vector< Acts::ExtrapolationCell< Acts::TrackParameters > > cCells;
  cCells.reserve(simulated.size());
  std::vector< Acts::ExtrapolationCell< Acts::NeutralParameters > > nCells;
  nCells.reserve(simulated.size());

  // loop over particles  
  for (const auto& particle : simulated) {
    // @todo, potentially update to better structure with Vertex-Particle tree
    Acts::PerigeeSurface surface(particle.vertex());
    double               d0    = 0.;
    double               z0    = 0.;
    double               phi   = particle.momentum().phi();
    double               theta = particle.momentum().theta();
    // treat differently for neutral particles
    double               qop   = particle.charge() != 0 ? 
                                 particle.charge() / particle.momentum().mag() : 
                                 1. / particle.momentum().mag();
    // parameters
    Acts::ActsVectorD<5> pars;
    pars << d0, z0, phi, theta, qop;
    // some screen output
    std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;
    // execute the test for charged particles
    if (particle.charge()){
      // charged extrapolation - with hit recording
      Acts::BoundParameters startParameters(
          std::move(cov), std::move(pars), surface);
      if (executeTestT<Acts::TrackParameters>(startParameters, 
                                              particle.barcode(),
                                              cCells,
                                              &hits)
          != FW::ProcessCode::SUCCESS)
          ACTS_VERBOSE("Test of charged parameter extrapolation did not succeed.");
    } else {     
      // neutral extrapolation
      Acts::NeutralBoundParameters neutralParameters(
            std::move(cov), std::move(pars), surface);
        // prepare hits for charged neutral paramters - no hit recording
        if (executeTestT<Acts::NeutralParameters>(neutralParameters,  
                                                  particle.barcode(),
                                                  nCells)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING(
              "Test of neutral parameter extrapolation did not succeed.");
    }
  }

  // write simulated data to the event store
  // - the particles
  if (ctx.eventStore.add(m_cfg.simulatedParticlesCollection,
                         std::move(simulated))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  // - the extrapolation cells - charged - if configured 
  if (m_cfg.simulatedChargedExCellCollection != "" 
      && ctx.eventStore.add(m_cfg.simulatedChargedExCellCollection, std::move(cCells))
      == FW::ProcessCode::ABORT) {
       return FW::ProcessCode::ABORT;
  }
  // - the extrapolation cells - neutral - if configured 
  if (m_cfg.simulatedNeutralExCellCollection != "" 
      && ctx.eventStore.add(m_cfg.simulatedNeutralExCellCollection, std::move(nCells))
      == FW::ProcessCode::ABORT) {
       return FW::ProcessCode::ABORT;
  }
  // - the simulated hits - if configured
  if (m_cfg.simulatedHitsCollection != "" 
      && ctx.eventStore.add(m_cfg.simulatedHitsCollection, std::move(hits))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }
  return FW::ProcessCode::SUCCESS;
}
