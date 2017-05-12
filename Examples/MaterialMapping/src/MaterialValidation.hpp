///////////////////////////////////////////////////////////////////
// MaterialValidation.hpp
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

/// @class MaterialValidation
///
/// @brief Writes out ACTS and original Layer material
///
/// The MaterialValidation writes out the material maps per layer in ACTS and
/// writes out the original material collected within the full detector
/// geometry.

class MaterialValidation : public FW::Algorithm
{
public:
  /// @class Config
  /// of the MaterialValidation

  struct Config : public FW::Algorithm::Config
  {
  public:
    /// The ACTS material mapper
    std::shared_ptr<Acts::MaterialMapping> materialMapper;
    /// The writer of the material
    std::shared_ptr<FW::IMaterialWriter>    materialWriter;
    /// The writer of the material steps
    std::shared_ptr<FW::IMaterialStepWriter> materialStepWriter;

    Config()
      : FW::Algorithm::Config("MaterialValidation")
      , materialMapper(nullptr)
      , materialWriter(nullptr)
      , materialStepWriter(nullptr)
    {
    }
  };

  /// Constructor
  MaterialValidation(const Config&                 cnf,
                    std::unique_ptr<Acts::Logger> logger
                    = Acts::getDefaultLogger("MaterialValidation",
                                             Acts::Logging::INFO));

  /// Destructor
  ~MaterialValidation();

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
  Config m_cfg;
};
}

#endif  // MATERIALMAPPINGTEST_LAYERMATERIALTEST_H
