///////////////////////////////////////////////////////////////////
// MaterialRootification.hpp
///////////////////////////////////////////////////////////////////

#ifndef MATERIALMAPPINGTEST_MATERIALROOTIFICATION_H
#define MATERIALMAPPINGTEST_MATERIALROOTIFICATION_H

#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IMaterialTrackRecReader.hpp"
#include "ACTFW/Writers/IMaterialTrackRecWriter.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace FWE {

/// @class MaterialRootification
///
/// @brief class to test MaterialTrackRecWriter and Reader
///

class MaterialRootification : public FW::Algorithm
{
public:
  /// @class Config
  /// Configuration of MaterialRootification
  struct Config : public FW::Algorithm::Config
  {
  public:
    /// The Writer
    std::shared_ptr<FW::IMaterialTrackRecWriter> materialTrackRecWriter;
    /// The Reader
    std::shared_ptr<FW::IMaterialTrackRecReader> materialTrackRecReader;

    Config()
      : FW::Algorithm::Config("MaterialRootification")
      , materialTrackRecWriter(nullptr)
      , materialTrackRecReader(nullptr)
    {
    }
  };

  /// Constructor
  MaterialRootification(const Config&                 cnf,
                        std::unique_ptr<Acts::Logger> logger
                        = Acts::getDefaultLogger("MaterialRootification",
                                                 Acts::Logging::INFO));

  /// Destructor
  ~MaterialRootification();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;

  /// Framework execute method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

private:
  /// The config object
  Config m_cfg;
};
}

#endif  // MATERIALMAPPINGTEST_MATERIALROOTIFICATION_H
