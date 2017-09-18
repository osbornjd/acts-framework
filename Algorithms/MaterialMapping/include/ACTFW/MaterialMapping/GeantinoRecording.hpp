///////////////////////////////////////////////////////////////////
// GeantinoRecording.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTW_ALGORITHMS_MATERIALMAPPING_GEANTINORECORDING_H
#define ACTW_ALGORITHMS_MATERIALMAPPING_GEANTINORECORDING_H

#include <memory>
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/GeometryInterfaces/IGeant4Service.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "G4RunManager.hh"

namespace FW {
class WhiteBoard;
}

namespace FW {

/// @class GeantinoRecording
///
/// @brief records the simulation geometry by using geantinos
///
/// The GeantinoRecording algorithm initiates the geant4 simulation
/// of the Geant4materialMapping plugin, creates and writes out
/// the MaterialTrack entities which are needed for material mapping.
/// The input for the geant4 geometry can be either provided by a gdml file
/// or an implementation of the IGeant4Service.

class GeantinoRecording : public FW::BareAlgorithm
{
public:
  /// @class Config
  struct Config
  {
    /// The writer writing out the MaterialTrack entities
    std::shared_ptr< FW::IWriterT<Acts::MaterialTrack> >
      materialTrackWriter = nullptr;
    /// The service possibly providing the Geant4 geometry (optional)
    /// @note If this is not set, the geometry should be given by gdml file
    std::shared_ptr<FW::IGeant4Service> geant4Service = nullptr;
    /// The possible gmdl input (optional)
    std::string gdmlFile;
    /// The number of tracks per event
    size_t tracksPerEvent = 0;
    /// random number seed 1
    int seed1 = 12345;
    /// random number seed 2
    int seed2 = 45678;
  };

  /// Constructor
  GeantinoRecording(const Config&        cnf,
                    Acts::Logging::Level level = Acts::Logging::INFO);

  FW::ProcessCode
  execute(FW::AlgorithmContext context) const final;

private:
  /// The config object
  Config          m_cfg;
  /// G4 run manager
  std::unique_ptr<G4RunManager> m_runManager;
};

}

#endif // ACTW_ALGORITHMS_GEANTINORECORDING_H
