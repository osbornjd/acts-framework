//
//  G4FatrasValidation.h
//  ACTFW

#ifndef ACTFW_GEANT4SIMULATION_G4FATRASVALIDATION_H
#define ACTFW_GEANT4SIMULATION_G4FATRASVALIDATION_H 1

#include <cmath>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/GeometryInterfaces/IGeant4Service.hpp"
#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "G4MTRunManager.hh"

namespace FW {
class WhiteBoard;
}

namespace Acts {
class ParticleProperties;
}

namespace FWG4 {

/// @class G4FatrasValidation
///
/// @brief Algorithm steering the Geant4 simulation for Fatras validation.
///
/// This class is needed to validate the Fatras simulation against Geant4.
/// It steers the Geant4 simulation and writes out the particle properties of
/// the first and last step in order to compare the energy loss or the
/// displacement.
///
class G4FatrasValidation : public FW::Algorithm
{
public:
  /// @class Config
  struct Config : public FW::Algorithm::Config
  {
    /// output writer
    std::shared_ptr<FW::IWriterT<std::pair<Acts::ParticleProperties,
                                           Acts::ParticleProperties>>>
        materialEffectsWriter = nullptr;
    /// the geant4geometry
    std::shared_ptr<FW::IGeant4Service> geant4Service = nullptr;
    /// number of tests per event
    size_t testsPerEvent = 1;
    /// the radial limit, where the last step should be collected
    double radialStepLimit = 0.;
    /// The configuration object of the PrimaryGeneratorAction
    FWG4::PrimaryGeneratorAction::Config pgaConfig;

    Config() : FW::Algorithm::Config("G4FatrasValidation") {}
  };

  /// Constructor
  G4FatrasValidation(const Config&                       cnf,
                     std::unique_ptr<const Acts::Logger> logger
                     = Acts::getDefaultLogger("G4FatrasValidation",
                                              Acts::Logging::INFO));

  /// Destructor
  ~G4FatrasValidation();

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
  /// the config class
  Config m_cfg;
  /// the geant4 runmanager
  G4MTRunManager* m_runManager;
};
}

#endif  // ACTFW_GEANT4SIMULATION_G4FATRASVALIDATION_H
