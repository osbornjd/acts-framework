///////////////////////////////////////////////////////////////////
// RootMaterialWriter.h
///////////////////////////////////////////////////////////////////

#ifndef ROOT_ROOTMATERIALWRITER_H
#define ROOT_ROOTMATERIALWRITER_H

#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/Writers/IMaterialWriter.hpp"
#include "ACTS/Material/SurfaceMaterial.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "TH2F.h"
#include "TTree.h"

namespace FWRoot {

/// @class RootMaterialWriter
///
/// @brief Writes out the material properties of a surface with
/// BinnedSurfaceMaterial onto a root tree
///
/// This service is the root implementation of the IMaterialWriter.
/// It writes out the material properties of a given surface which has
/// BinnedSurfaceMaterial onto a root tree.

class RootMaterialWriter : public FW::IMaterialWriter {
 public:
  /// @class Config
  /// Configuration of the Writer
  class Config {
   public:
    /// The default logger
    std::shared_ptr<Acts::Logger> logger;
    /// The name of the service
    std::string name;
    /// The name of the output file
    std::string fileName;

    Config(const std::string& lname = "MaterialWriter",
           Acts::Logging::Level lvl = Acts::Logging::INFO)
        : logger(Acts::getDefaultLogger(lname, lvl)),
          name(lname),
          fileName("") {}
  };

  /// Constructor
  RootMaterialWriter(const Config& cfg);

  /// Destructor
  virtual ~RootMaterialWriter() = default;

  /// Framework intialize method
  FW::ProcessCode initialize() final;

  /// Framework finalize method
  FW::ProcessCode finalize() final;

  /// Interface method which writes out the material properties
  FW::ProcessCode write(
      const Acts::SurfaceMaterial& material,
      const Acts::GeometryID& geoID, 
      const std::string& name) final;

  /// Framework name() method
  const std::string& name() const final;

 private:
  /// The config class
  Config m_cfg;
  /// The output file name
  TFile* m_outputFile;

  /// Private access to the logging instance
  const Acts::Logger& logger() const { return *m_cfg.logger; }
};

inline const std::string& RootMaterialWriter::name() const {
  return m_cfg.name;
}
}

#endif  // ROOT_ROOTMATERIALWRITER_H
