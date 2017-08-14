#ifndef ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H
#define ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H

#include <array>

#include <ACTS/Utilities/Units.hpp>

#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {

class BarcodeSvc;
class RandomNumbersSvc;

/// @class ParticleGun
///
/// Particle gun implementation of the IParticleReader. It can be used as
/// particle input for the Fatras example.
/// It generates particles of given type with random momentum and random vertex
/// in a given range. It fills a vector of particle properties for feeding into
/// fast simulation.
///
class ParticleGun : public BareAlgorithm
{
public:
  struct Config
  {
    /// output collection for generated particles
    std::string particlesCollection;
    /// number of particles
    size_t nParticles = 0;
    /// low, high for d0 range
    std::array<double, 2> d0Range = {{0, 1 * Acts::units::_mm}};
    /// low, high for z0 range
    std::array<double, 2> z0Range
        = {{-100 * Acts::units::_mm, 100 * Acts::units::_mm}};
    /// low, high for phi range
    std::array<double, 2> phiRange = {{-M_PI, M_PI}};
    /// low, high for eta range
    std::array<double, 2> etaRange = {{-3., 3.}};
    /// low, high for pt range
    std::array<double, 2> ptRange
        = {{100 * Acts::units::_MeV, 10 * Acts::units::_GeV}};
    /// the mass of the particle
    double mass = 0.;
    /// the charge of the particle
    double charge = 0.;
    /// the pdg type of the particle
    pdg_type pID = 0.;
    // FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    std::shared_ptr<FW::BarcodeSvc> barcodes = nullptr;
  };

  /// Constructor
  /// @param cfg is the configuration class
  ParticleGun(const Config&        cfg,
              Acts::Logging::Level level = Acts::Logging::INFO);

  ProcessCode
  execute(AlgorithmContext ctx) const;

private:
  Config m_cfg;
};

}  // namespace FW

#endif  // ACTFW_ALGORITHMS_FATRAS_PARTICLEGUN_H
