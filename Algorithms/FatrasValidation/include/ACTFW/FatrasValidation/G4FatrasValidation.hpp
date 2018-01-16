// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_GEANT4SIMULATION_G4FATRASVALIDATION_H
#define ACTFW_GEANT4SIMULATION_G4FATRASVALIDATION_H 1

#include <cmath>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/BareAlgorithm.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTFW/GeometryInterfaces/IGeant4Service.hpp"
#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "ACTS/Utilities/GeometryID.hpp"
#include "G4MTRunManager.hh"

namespace Acts {
class ParticleProperties;
}

namespace FW {

class WhiteBoard;

namespace G4 {

  /// @class G4FatrasValidation
  ///
  /// @brief Algorithm steering the Geant4 simulation for Fatras validation.
  ///
  /// This class is needed to validate the Fatras simulation against Geant4.
  /// It steers the Geant4 simulation and writes out the particle properties of
  /// the first and last step in order to compare the energy loss or the
  /// displacement.
  ///
  class G4FatrasValidation : public BareAlgorithm
  {
  public:
    /// @class Config
    struct Config
    {
      /// particle properties of first and last particles to write
      std::string particlePropertiesCollection;
      /// the geant4geometry
      std::shared_ptr<FW::IGeant4Service> geant4Service = nullptr;
      /// number of tests per event
      size_t testsPerEvent = 1;
      /// the radial limit, where the last step should be collected
      double radialStepLimit = 0.;
      /// The configuration object of the PrimaryGeneratorAction
      PrimaryGeneratorAction::Config pgaConfig;
    };

    /// Constructor
    G4FatrasValidation(const Config&        cnf,
                       Acts::Logging::Level loglevel = Acts::Logging::INFO);

    /// Destructor
    ~G4FatrasValidation();

    /// Framework execode method
    FW::ProcessCode
    execute(const AlgorithmContext context) const override final;

  private:
    /// the config class
    Config m_cfg;
    /// the geant4 runmanager
    G4MTRunManager* m_runManager;
  };
}  // end of namespace G4
}  // end of namespace FW
#endif  // ACTFW_GEANT4SIMULATION_G4FATRASVALIDATION_H
