#include <cmath>
#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Utilities/Units.hpp"

FW::ParticleGun::ParticleGun(const FW::ParticleGun::Config& cfg,
                             Acts::Logging::Level           level)
  : FW::BareAlgorithm("ParticleGun", level), m_cfg(cfg)
{}

FW::ProcessCode
FW::ParticleGun::execute(AlgorithmContext ctx) const
{

  ACTS_DEBUG("::execute() called for event " << ctx.eventNumber );
  // what's written out
  std::vector<Acts::ProcessVertex> vertices;
  RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);

  UniformDist d0Dist(m_cfg.d0Range.at(0), m_cfg.d0Range.at(1));
  UniformDist z0Dist(m_cfg.z0Range.at(0), m_cfg.z0Range.at(1));
  UniformDist phiDist(m_cfg.phiRange.at(0), m_cfg.phiRange.at(1));
  UniformDist etaDist(m_cfg.etaRange.at(0), m_cfg.etaRange.at(1));
  UniformDist ptDist(m_cfg.ptRange.at(0), m_cfg.ptRange.at(1));
  UniformDist chargeDist(0.,1.);
  
  
  // the particles
  std::vector<Acts::ParticleProperties> particles;
  for (size_t ip = 0; ip < m_cfg.nParticles; ip++) {
    // generate random parameters
    double d0  = d0Dist(rng);
    double z0  = z0Dist(rng);
    double phi = phiDist(rng);
    double eta = etaDist(rng);
    double pt  = ptDist(rng);
    auto   bc  = m_cfg.barcodes->generate(ip);
    // create vertex from random parameters
    Acts::Vector3D vertex(d0 * std::sin(phi), d0 * -std::cos(phi), z0);
    // create momentum from random parameters
    Acts::Vector3D momentum(
        pt * std::cos(phi), pt * std::sin(phi), pt * std::sinh(eta));
    // flip charge and PID if asked for
    int flip = (!m_cfg.randomCharge || chargeDist(rng) < 0.5) ? 1 : -1;
    // the particle should be ready now
    particles.emplace_back(momentum, 
                           m_cfg.mass, 
                           flip*m_cfg.charge, 
                           flip*m_cfg.pID, 
                           bc);
  }
  ACTS_DEBUG("Generated 1 vertex with " << particles.size() << " particles.");                                       
  // the vertices
  vertices.push_back(Acts::ProcessVertex(Acts::Vector3D(0.,0.,0.),
                                         0.,
                                         0,
                                         {},
                                         std::move(particles)));
  if (ctx.eventStore.add(m_cfg.evgenCollection, std::move(vertices))
      != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
