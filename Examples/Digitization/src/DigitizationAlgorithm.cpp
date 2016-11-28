#include "ACTFW/Extrapolation/DigitizationAlgorithm.hpp"
#include <iostream>
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"

FWE::DigitizationAlgorithm::DigitizationAlgorithm(
    const FWE::DigitizationAlgorithm::Config& cfg,
    std::unique_ptr<Acts::Logger>                  logger)
  : FW::Algorithm(cfg, std::move(logger)), m_cfg(cfg)
{
}

FWE::DigitizationAlgorithm::~DigitizationAlgorithm()
{}

/** Framework finalize mehtod */
FW::ProcessCode
FWE::DigitizationAlgorithm::initialize(
    std::shared_ptr<FW::WhiteBoard> jStore)
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
FWE::DigitizationAlgorithm::execute(
    const FW::AlgorithmContext context) const
{
  // we read from a collection
  if (m_cfg.particleCollectionName != "") {
    // Retrieve relevant information from the execution context
    auto eventStore = context.eventContext->eventStore;

    // prepare the input vector
    std::vector<Acts::ParticleProperties>* eventParticles = nullptr;
    // read and go
    if (eventStore
        && eventStore->readT(eventParticles, m_cfg.particleCollectionName)
            == FW::ProcessCode::ABORT)
      return FW::ProcessCode::ABORT;
    // run over it
    ACTS_INFO("Successfully read in collection with " << eventParticles->size()
                                                      << " particles");

    size_t pCounter = 0;
    size_t sCounter = 0;
    for (auto& eParticle : (*eventParticles)) {
      // process the particle // TODO make configuraable
      if (eParticle.charge() != 0.
          && eParticle.vertex().perp() < 1.
          && eParticle.momentum().perp() > m_cfg.minPt
          && fabs(eParticle.momentum().eta()) < m_cfg.maxEta ){
        // count simulated particles    
        ++pCounter;    
        // TODO update to better structure with Vertex-Particle tree
        Acts::PerigeeSurface surface(eParticle.vertex());
        double d0    = 0.;
        double z0    = 0.;
        double phi   = eParticle.momentum().phi();
        double theta = eParticle.momentum().theta();
        double qop   = eParticle.charge()/eParticle.momentum().mag();
        // parameters
        Acts::ActsVectorD<5> pars;
        pars << d0, z0, phi, theta, qop;
        // some screen output
        std::unique_ptr<Acts::ActsSymMatrixD<5>> cov = nullptr;

        Acts::BoundParameters startParameters(std::move(cov), std::move(pars), surface);
        if (executeTestT<Acts::TrackParameters>(startParameters) != FW::ProcessCode::SUCCESS)
            ACTS_VERBOSE("Test of parameter extrapolation did not succeed.");
      } else 
        ++sCounter;
    }
    ACTS_INFO("Number of simulated particles : " <<  pCounter);
    ACTS_INFO("Number of skipped   particles : " <<  sCounter);
    
  } else {

    // Create a random number generator
    FW::RandomNumbersSvc::Generator rng
        = m_cfg.randomNumbers->spawnGenerator(context);

    // loop
    for (size_t iex = 0; iex < m_cfg.testsPerEvent; ++iex) {
      // gaussian d0 and z0
      double d0    = drawGauss(rng, m_cfg.d0Defs);
      double z0    = drawGauss(rng, m_cfg.z0Defs);
      double phi   = drawUniform(rng, m_cfg.phiRange);
      double eta   = drawUniform(rng, m_cfg.etaRange);
      double theta = 2. * atan(exp(-eta));
      double pt    = drawUniform(rng, m_cfg.ptRange);
      double p     = pt / sin(theta);
      double q     = drawUniform(rng, {{0., 1.}}) > 0.5 ? 1. : -1.;

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
        Acts::BoundParameters startParameters(
            std::move(cov), std::move(pars), pSurface);
        if (executeTestT<Acts::TrackParameters>(startParameters)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING("Test of parameter extrapolation did not succeed.");

      } else {
        // charged extrapolation
        Acts::NeutralBoundParameters startParameters(
            std::move(cov), std::move(pars), pSurface);
        if (executeTestT<Acts::NeutralParameters>(startParameters)
            != FW::ProcessCode::SUCCESS)
          ACTS_WARNING("Test of parameter extrapolation did not succeed.");
      }
    }
  }
  // return SUCCESS to the frameword
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
FWE::DigitizationAlgorithm::finalize()
{
  ACTS_VERBOSE("initialize successful.");
  return FW::ProcessCode::SUCCESS;
}

double
FWE::DigitizationAlgorithm::drawGauss(
    FW::RandomNumbersSvc::Generator& rng,
    const std::array<double, 2>& pars) const
{
  double mean  = pars[0];
  double sigma = pars[1];
  return mean + rng.draw(FW::Distribution::gauss) * sigma;
}

double
FWE::DigitizationAlgorithm::drawUniform(
    FW::RandomNumbersSvc::Generator& rng,
    const std::array<double, 2>& range) const
{
  double low   = range[0];
  double high  = range[1];
  double delta = high - low;
  return low + rng.draw(FW::Distribution::uniform) * delta;
}
