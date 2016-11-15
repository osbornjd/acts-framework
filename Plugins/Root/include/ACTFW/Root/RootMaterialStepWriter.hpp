///////////////////////////////////////////////////////////////////
// RootMaterialStepWriter.hpp
///////////////////////////////////////////////////////////////////

#ifndef ROOT_ROOTMATERIALSTEPWRITER_HPP
#define ROOT_ROOTMATERIALSTEPWRITER_HPP

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IMaterialStepWriter.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialStep.hpp"
#include "ACTS/Surfaces/Surface.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "TFile.h"
#include "TH2F.h"

namespace FWRoot {

/// @class RootMaterialStepWriter
///
/// @brief Writes out MaterialSteps per surface object into a root tree
///
/// This service is the root implementation of the IMaterialStepWriter.
/// It writes out a  root tree for every surface object containing the material
/// properties on the surface with its corresponding positions.

class RootMaterialStepWriter : public FW::IMaterialStepWriter
{
public:
  /// @class Config
  /// Configuration of the Writer
  class Config
  {
  public:
    /// The default logger
    std::shared_ptr<Acts::Logger> logger;
    /// The name of the service
    std::string name;
    /// The name of the output file
    std::string fileName;

    Config(const std::string&   lname = "MaterialWriter",
           Acts::Logging::Level lvl   = Acts::Logging::INFO)
      : logger(Acts::getDefaultLogger(lname, lvl)), name(lname), fileName("")
    {
    }
  };

  /// Constructor
  RootMaterialStepWriter(const Config& cfg);

  /// Destructor
  virtual ~RootMaterialStepWriter() = default;

  /// Framework intialize method
  FW::ProcessCode
  initialize() final;

  /// Framework finalize method
  FW::ProcessCode
  finalize() final;

  /// Interface method which writes out the MaterialStep entities
  FW::ProcessCode
  write(std::string                     name,
        const Acts::Surface*            surface,
        std::vector<Acts::MaterialStep> msteps) final;

  /// Framework name() method
  const std::string&
  name() const final;

private:
  /// The config class
  Config m_cfg;
  /// The output file name
  TFile* m_outputFile;

  /// Private access to the logging instance
  const Acts::Logger&
  logger() const
  {
    return *m_cfg.logger;
  }
};

inline const std::string&
RootMaterialStepWriter::name() const
{
  return m_cfg.name;
}
}

#endif  // ROOT_ROOTMATERIALSTEPWRITER_HPP
