#include "ACTFW/Fatras/ParticleGun.hpp"

#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Utilities/Units.hpp"

FW::ParticleGun::ParticleGun(const FW::ParticleGun::Config& cfg,
                             Acts::Logging::Level           level)
  : FW::BareAlgorithm("ParticleGun", level), m_cfg(cfg)
{
}

FW::ProcessCode
FW::ParticleGun::execute(AlgorithmContext ctx) const
{
  std::vector<Acts::ParticleProperties> particles;

  RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(ctx);

  UniformDist phiDist(m_cfg.phiRange.at(0), m_cfg.phiRange.at(1));
  UniformDist etaDist(m_cfg.etaRange.at(0), m_cfg.etaRange.at(1));
  UniformDist ptDist(m_cfg.ptRange.at(0), m_cfg.ptRange.at(1));

  for (size_t ip = 0; ip < m_cfg.nParticles; ip++) {
    // generate random parameters
    double phi   = phiDist(rng);
    double eta   = etaDist(rng);
    double theta = 2. * atan(exp(-eta));
    double pt    = ptDist(rng);
    double p     = pt / sin(theta);
    // create momentum from random parameters
    Acts::Vector3D momentum(
        p * sin(theta) * cos(phi), p * sin(theta) * sin(phi), p * cos(theta));
    // create vertex from random parameters
    Acts::Vector3D vertex(0., 0., 0.);
    // the particle should be ready now
    particles.emplace_back(
        vertex, momentum, m_cfg.mass, m_cfg.charge, m_cfg.pID);
  }

  if (ctx.eventStore.add(m_cfg.particlesCollection, std::move(particles))
      != ProcessCode::SUCCESS)
    return ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
