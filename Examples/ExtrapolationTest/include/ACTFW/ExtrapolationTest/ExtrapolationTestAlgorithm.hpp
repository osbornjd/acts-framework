//
//  ExtrapolationTestAlgorithm.h
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//
//

#ifndef ACTFW_EXAMPLES_EXTRAPOLATIONTESTALGORITHM_H
#define ACTFW_EXAMPLES_EXTRAPOLATIONTESTALGORITHM_H 1

#include <cmath>
#include <memory>

#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace Acts {
class IExtrapolationEngine;
}

namespace FW {
class WhiteBoard;
class RandomNumbers;
class IExtrapolationCellWriter;
}

namespace FWE {

/// @class Algorithm
class ExtrapolationTestAlgorithm : public FW::Algorithm
{
public:
  /// @class Config
  struct Config : public FW::Algorithm::Config
  {
    /// FW random number service
    std::shared_ptr<FW::RandomNumbers> randomNumbers = nullptr;
    /// the extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;
    /// output writer
    std::shared_ptr<FW::IExtrapolationCellWriter> extrapolationCellWriter
        = nullptr;
    /// number of tests per event
    size_t testsPerEvent = 1;
    /// parameter type : 0 = neutral | 1 = charged
    int parameterType = 1;
    /// mean, sigma for d0 range
    std::array<double, 2> d0Defs = {{0., 2.}};
    /// mean, sigma for z0 range
    std::array<double, 2> z0Defs = {{0., 50.}};
    /// low, high for eta range
    std::array<double, 2> etaRange = {{-3., 3.}};
    /// low, high for phi range
    std::array<double, 2> phiRange = {{-M_PI, M_PI}};
    /// low, high for pt range
    std::array<double, 2> ptRange = {{100., 10000.}};
    /// particle type definition
    bool particleType = true;
    /// configuration: sensitive collection
    bool collectSensitive = true;
    /// configuration: collect passive
    bool collectPassive = true;
    /// configuration: collect boundary
    bool collectBoundary = true;
    /// configuration: collect material
    bool collectMaterial = true;
    /// configuration: don't collapse
    bool sensitiveCurvilinear = false;
    /// define how robust the search mode is
    int searchMode = 0;
    /// set the patch limit of the extrapolation
    double pathLimit = -1.;

    Config() : FW::Algorithm::Config("ExtrapolationTestAlgorithm") {}
  };

  /// Constructor
  ExtrapolationTestAlgorithm(
      const Config&                 cnf,
      std::unique_ptr<Acts::Logger> logger
      = Acts::getDefaultLogger("ExtrapolationTestAlgorithm",
                               Acts::Logging::INFO));

  /// Destructor
  ~ExtrapolationTestAlgorithm();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) override final;

  /// Framework execode method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() override final;

private:
  Config m_cfg;  //!< the config class

  double
  drawGauss(const std::array<double, 2>& range) const;
  double
  drawUniform(const std::array<double, 2>& range) const;

  template <class T>
  FW::ProcessCode
  executeTestT(const T& startParameters) const;
};

#include "ExtrapolationTestAlgorithm.ipp"
}

#endif
