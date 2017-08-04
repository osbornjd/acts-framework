#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Utilities/Units.hpp"

FWE::ParticleGun::ParticleGun(const FWE::ParticleGun::Config&     cfg,
                              std::unique_ptr<const Acts::Logger> mlogger)
  : FW::IReaderT<std::vector<Acts::ParticleProperties>>()
  , m_cfg(cfg)
  , m_logger(std::move(mlogger))
{
  if (!m_cfg.randomNumbers) {
    ACTS_FATAL("ParticleGun constructor did not succeed! No RandomNumbersSvc "
               "handed over!");
  }
}

FWE::ParticleGun::~ParticleGun()
{
}

FW::ProcessCode
FWE::ParticleGun::read(
    std::vector<Acts::ParticleProperties>& particleProperties,
    size_t                                 skip,
    const FW::AlgorithmContext*            context)
{
  if (!context) {
    ACTS_FATAL("read() did not succeed! No AlgorithmContext "
               "handed over!");
    return FW::ProcessCode::ABORT;
  }

  // Create a random number generator
  FW::RandomEngine rng = m_cfg.randomNumbers->spawnGenerator(*context);

  // Create distributions in given ranges
  FW::UniformDist phiDist(m_cfg.phiRange.at(0), m_cfg.phiRange.at(1));
  FW::UniformDist etaDist(m_cfg.etaRange.at(0), m_cfg.etaRange.at(1));
  FW::UniformDist ptDist(m_cfg.ptRange.at(0), m_cfg.ptRange.at(1));

  // Particle loop
  for (int ip = 0; ip < m_cfg.nParticles; ip++) {
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
    particleProperties.push_back(Acts::ParticleProperties(
        vertex, momentum, m_cfg.mass, m_cfg.charge, m_cfg.pID));
  }
  // return
  return FW::ProcessCode::SUCCESS;
}

std::string
FWE::ParticleGun::name() const
{
  return "ParticleGun";
}

FW::ProcessCode
FWE::ParticleGun::initialize()
{
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::ParticleGun::finalize()
{
  return FW::ProcessCode::SUCCESS;
}
