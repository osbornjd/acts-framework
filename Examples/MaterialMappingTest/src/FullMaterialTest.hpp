///////////////////////////////////////////////////////////////////
// FullMaterialTest.hpp
///////////////////////////////////////////////////////////////////

#ifndef MATERIALMAPPINGTEST_FULLMATERIALTEST_H
#define MATERIALMAPPINGTEST_FULLMATERIALTEST_H

#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IMaterialTrackRecWriter.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace Acts {
class MaterialMapping;
class IExtrapolationEngine;
}

namespace FWE {

/// @class FullMaterialTest
///
/// @brief Writes out the full material
///
/// The FullMaterialTest writes out the full material in ACTS of the Tracking
/// Geometry.

class FullMaterialTest : public FW::Algorithm
{
public:
  /// @class Config
  /// of the FullMaterialTest

  struct Config : public FW::Algorithm::Config
  {
  public:
    /// The writer of the material
    std::shared_ptr<FW::IMaterialTrackRecWriter> materialTrackRecWriter
        = nullptr;
    /// FW random number service
    std::shared_ptr<FW::RandomNumbersSvc> randomNumbers = nullptr;
    /// The extrapolation engine
    std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine = nullptr;

    Config() : FW::Algorithm::Config("FullMaterialTest") {}
  };

  /// Constructor
  FullMaterialTest(const Config&                 cnf,
                   std::unique_ptr<Acts::Logger> logger
                   = Acts::getDefaultLogger("FullMaterialTest",
                                            Acts::Logging::INFO));

  /// Destructor
  ~FullMaterialTest();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;

  /// Framework execute method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const final;

  /// Framework finalize method
  FW::ProcessCode
  finalize() final;

private:
  /// The config object
  Config m_cnf;
};
}

#endif  // MATERIALMAPPINGTEST_FULLMATERIALTEST_H
