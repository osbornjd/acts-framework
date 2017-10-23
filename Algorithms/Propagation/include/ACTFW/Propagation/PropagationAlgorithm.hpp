//
//  PropagationAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//
#ifndef ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
#define ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H

#include <cmath>
#include <limits>
#include <memory>
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumberDistributions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/EventData/TrackParameters.hpp"
#include "ACTS/Extrapolation/ExtrapolationCell.hpp"
#include "ACTS/Surfaces/CylinderSurface.hpp"
#include "ACTS/Surfaces/PerigeeSurface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Units.hpp"

typedef std::unique_ptr<const Acts::TrackParameters> tp_ptr;

#define propagateAB(prop, pars, opt, res)                                      \
  {                                                                            \
    tp_ptr p = prop->propagate(pars, opt).endParameters;                       \
    if (p) {                                                                   \
      const auto& pos = p->position();                                         \
      ACTS_VERBOSE("Intersection = " << pos.x() << "," << pos.y() << ","       \
                                     << pos.z())                               \
      res.push_back(std::move(p));                                             \
    }                                                                          \
  }

#define propagateSfAB(prop, pars, sf, opt, res)                                \
  {                                                                            \
    tp_ptr p = prop->propagate(pars, sf, opt).endParameters;                   \
    if (p) {                                                                   \
      const auto& pos = p->position();                                         \
      ACTS_VERBOSE("Intersection = " << pos.x() << "," << pos.y() << ","       \
                                     << pos.z())                               \
      res.push_back(std::move(p));                                             \
    }                                                                          \
  }

#define propagateCacheAB(prop, pars, sf, cache, opt, res)                      \
  {                                                                            \
    tp_ptr p = prop->propagate_with_cache(cache, pars, sf, opt).endParameters; \
    if (p) {                                                                   \
      const auto& pos = p->position();                                         \
      ACTS_VERBOSE("Intersection = " << pos.x() << "," << pos.y() << ","       \
                                     << pos.z())                               \
      res.push_back(std::move(p));                                             \
    }                                                                          \
  }

#define propagateE(prop, pars, sf, res)                                        \
  {                                                                            \
    Acts::ExtrapolationCell<Acts::TrackParameters> ec(pars);                   \
    ec.pathLimit = m_cfg.pathLimit;                                            \
    auto statusB = prop->propagate(ec, sf);                                    \
    if (ec.endParameters) {                                                    \
      const auto& pos = ec.endParameters->position();                          \
      ACTS_VERBOSE("Intersection = " << pos.x() << "," << pos.y() << ","       \
                                     << pos.z());                              \
      res.push_back(std::move(ec.endParameters));                              \
    }                                                                          \
  }

/// @brief this test algorithm performs test propagation
/// with the Acts::propagation::Propagator(s) and compares
/// them to the Acts::IPropagationEngine
///
/// There are three test modes availaible that run exclusively
/// - pathLength test, i.e. propagation starts from curvilinear
///   to curvilinear and stops at a certain path length
/// - surface, i.e. propagation from surface to another surface
/// - kalman, i.e. propagation in a sequential manner
namespace FW {

template <typename PropagatorA, typename PropagatorB, typename PropagatorE>
class PropagationAlgorithm : public BareAlgorithm
{
public:
  // the test mode for this propagation test
  enum TestMode { pathLength = 0, surface = 1, kalman = 2 };

  struct Config
  {
    /// the particles input collections
    std::string evgenCollection = "";
    // output collection to be written
    std::string trackParametersCollection = "";
    /// @todo remove later and replace by particle selector
    double maxD0  = std::numeric_limits<double>::max();
    double maxEta = std::numeric_limits<double>::max();
    double minPt  = 0.0;
    /// the propagors to be tested
    std::shared_ptr<PropagatorA> propagatorA = nullptr;
    std::shared_ptr<PropagatorB> propagatorB = nullptr;
    std::shared_ptr<PropagatorE> propagatorE = nullptr;
    /// the pathLimit test
    double pathLimit = std::numeric_limits<double>::max();
    /// the radii for testing
    std::vector<double> cylinderRadii;
    /// which type to be done
    bool cacheCall = true;
    /// the mode how to run this tests
    TestMode testMode = pathLength;
    /// covariance transport
    bool covarianceTransport = true;
    /// the covariance values
    Acts::ActsVectorD<5> covariances = Acts::ActsVectorD<5>::Zero();
    /// the correlation terms
    Acts::ActsSymMatrixD<5> correlations = Acts::ActsSymMatrixD<5>::Identity();
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
  };

  /// Constructor
  /// @param [in] cnf is the configuration struct
  /// @param [in] loglevel is the loggin level
  PropagationAlgorithm(const Config& cnf, Acts::Logging::Level loglevel);

  /// Framework execute method
  /// @param [in] the algorithm context for event consistency
  FW::ProcessCode
  execute(AlgorithmContext ctx) const final override;

private:
  /// private helper method to create a corrleated covariance matrix
  std::unique_ptr<Acts::ActsSymMatrixD<5>>
  generateCovariance(FW::RandomEngine& rnd, FW::GaussDist& gauss) const;

  Config m_cfg;  ///< the config class

  /// the run options
  typename PropagatorA::template Options<> m_optionsA;
  typename PropagatorB::template Options<> m_optionsB;

  /// the test surface
  std::unique_ptr<Acts::CylinderSurface> m_surface = nullptr;

  /// the test surfaces for the Kalman Fitter emulation
  std::vector<std::unique_ptr<Acts::CylinderSurface>> m_radialSurfaces;
};

#include "PropagationAlgorithm.ipp"

}  // namespace FW

#endif  // ACTFW_ALGORITHMS_EXTRAPOLATIONALGORITHM_H
