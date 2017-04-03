///////////////////////////////////////////////////////////////////
// SurfaceMaterialTest.hpp
///////////////////////////////////////////////////////////////////

#ifndef MATERIALMAPPINGTEST_LAYERMATERIALTEST_H
#define MATERIALMAPPINGTEST_LAYERMATERIALTEST_H

#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IMaterialStepWriter.hpp"
#include "ACTFW/Writers/IMaterialWriter.hpp"
#include "ACTS/Layers/Layer.hpp"
#include "ACTS/Plugins/MaterialPlugins/SurfaceMaterialRecord.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace Acts {
class MaterialMapping;
}

namespace FWE {

/// @class SurfaceMaterialTest
///
/// @brief Writes out ACTS and original Layer material
///
/// The SurfaceMaterialTest writes out the material maps per layer in ACTS and
/// writes out the original material collected within the full detector
/// geometry.

class SurfaceMaterialTest : public FW::Algorithm
{
public:
  /// @class Config
  /// of the SurfaceMaterialTest

  struct Config : public FW::Algorithm::Config
  {
  public:
    /// The ACTS material mapper
    std::shared_ptr<Acts::MaterialMapping> materialMapper;
    /// The writer of the material
    std::shared_ptr<FW::IMaterialWriter> materialWriter;
    /// The writer of the material steps
    std::shared_ptr<FW::IMaterialStepWriter> materialStepWriter;

    Config()
      : FW::Algorithm::Config("SurfaceMaterialTest")
      , materialMapper(nullptr)
      , materialWriter(nullptr)
      , materialStepWriter(nullptr)
    {
    }
  };

  /// Constructor
  SurfaceMaterialTest(const Config&                 cnf,
                    std::unique_ptr<Acts::Logger> logger
                    = Acts::getDefaultLogger("SurfaceMaterialTest",
                                             Acts::Logging::INFO));

  /// Destructor
  ~SurfaceMaterialTest();

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

#endif  // MATERIALMAPPINGTEST_LAYERMATERIALTEST_H
