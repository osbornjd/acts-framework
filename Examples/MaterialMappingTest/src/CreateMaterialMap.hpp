///////////////////////////////////////////////////////////////////
// CreateMaterialMap.hpp
///////////////////////////////////////////////////////////////////

#ifndef MATERIALMAPPINGTEST_CREATEMATERIALMAP_H
#define MATERIALMAPPINGTEST_CREATEMATERIALMAP_H

#include <memory>
#include "ACTFW/DD4hepPlugin/GeometryService.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/GeometryInterfaces/IGeant4Service.hpp"
#include "ACTFW/Writers/IMaterialTrackRecWriter.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace FW {
class WhiteBoard;
}

namespace FWE {

/// @class CreateMaterialMap
///
/// @brief Creates the material maps with Geant4
///
/// The CreateMaterialMap algorithm initiates the geant4 simulation
/// of the Geant4materialMapping plugin, creates and writes out
/// the MaterialTrackRecord entities which are needed for material mapping.
/// The input for the geant4 geometry can be either provided by a gdml file
/// or an implementation of the IGeant4Service.

class CreateMaterialMap : public FW::Algorithm
{
public:
  /** @class Config */
  struct Config : public FW::Algorithm::Config
  {
    /// The writer writing out the MaterialTrackRecord entities
    std::shared_ptr<FW::IMaterialTrackRecWriter> materialTrackRecWriter;
    /// The service possibly providing the Geant4 geometry
    /// @note if not set the geometry should be given by gdml file
    std::shared_ptr<FW::IGeant4Service> geant4Service;
    /// The possible gmdl input
    std::string gdmlFile;
    /// The number of events which will be processed
    size_t numberOfEvents;

    Config()
      : FW::Algorithm::Config("CreateMaterialMap")
      , materialTrackRecWriter(nullptr)
      , geant4Service(nullptr)
      , gdmlFile("")
      , numberOfEvents(0)
    {
    }
  };

  /// Constructor
  CreateMaterialMap(const Config&                 cnf,
                    std::unique_ptr<Acts::Logger> logger
                    = Acts::getDefaultLogger("CreateMaterialMap",
                                             Acts::Logging::INFO));

  /// Destructor
  ~CreateMaterialMap();

  /// Framework intialize method
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) final;

  /// Framework execode method
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() final;

private:
  /// The config object
  Config m_cnf;
};
}

#endif  // MATERIALMAPPINGTEST_CREATEMATERIALMAP_H
