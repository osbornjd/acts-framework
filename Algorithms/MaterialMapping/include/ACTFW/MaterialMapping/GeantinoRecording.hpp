///////////////////////////////////////////////////////////////////
// GeantinoRecording.hpp
///////////////////////////////////////////////////////////////////

#ifndef ACTW_ALGORITHMS_MATERIALMAPPING_GEANTINORECORDING_H
#define ACTW_ALGORITHMS_MATERIALMAPPING_GEANTINORECORDING_H

#include <memory>
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/GeometryInterfaces/IGeant4Service.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/Logger.hpp"
#include "ACTS/Plugins/MaterialPlugins/MaterialTrack.hpp"
#include "G4RunManager.hh"

namespace FW {
class WhiteBoard;
}

namespace FWA {

/// @class GeantinoRecording
///
/// @brief records the simulation geometry by using geantinos
///
/// The GeantinoRecording algorithm initiates the geant4 simulation
/// of the Geant4materialMapping plugin, creates and writes out
/// the MaterialTrack entities which are needed for material mapping.
/// The input for the geant4 geometry can be either provided by a gdml file
/// or an implementation of the IGeant4Service.

class GeantinoRecording : public FW::Algorithm
{
public:
  /// @class Config 
  struct Config : public FW::Algorithm::Config
  {
    /// The writer writing out the MaterialTrack entities
    std::shared_ptr< FW::IWriterT<Acts::MaterialTrack> > 
      materialTrackWriter;
    /// The service possibly providing the Geant4 geometry
    /// @note if not set the geometry should be given by gdml file
    std::shared_ptr<FW::IGeant4Service> geant4Service;
    /// The possible gmdl input
    std::string gdmlFile;
    /// The number of tracks per event
    size_t tracksPerEvent;
    /// random number seed 1
    int seed1;
    /// random number seed 2
    int seed2;

    Config()
      : FW::Algorithm::Config("GeantinoRecording")
      , materialTrackWriter(nullptr)
      , geant4Service(nullptr)
      , gdmlFile("")
      , tracksPerEvent(0)
      , seed1(12345)
      , seed2(45678)  
    {
    }
  };

  /// Constructor
  GeantinoRecording(const Config&                 cnf,
                    std::unique_ptr<Acts::Logger> logger
                    = Acts::getDefaultLogger("GeantinoRecording",
                                             Acts::Logging::INFO));

  /// Destructor
  ~GeantinoRecording();

  /// Framework intialize method
  /// @param jobStore is the store that lives the entire job
  FW::ProcessCode
  initialize(std::shared_ptr<FW::WhiteBoard> jobStore = nullptr) 
  override final;

  /// Framework execode method
  /// @param context is the Algorithm context for multithreading
  FW::ProcessCode
  execute(const FW::AlgorithmContext context) const 
  override final;

  /// Framework finalize mehtod
  FW::ProcessCode
  finalize() 
  override final;

private:
  /// The config object
  Config          m_cfg;
  
  /// G4 run manager  
  G4RunManager*   m_runManager;
  
};
}

#endif // ACTW_ALGORITHMS_GEANTINORECORDING_H
