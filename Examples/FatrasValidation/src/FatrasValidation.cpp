///////////////////////////////////////////////////////////////////
// Geant4SimulationTest.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/FatrasValidation/G4FatrasValidation.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/DD4hep/GeometryService.hpp"
#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/Geant4/PrimaryGeneratorAction.hpp"
#include "ACTFW/Plugins/Root/MaterialEffectsWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Utilities/Units.hpp"

/// This example initializes and processes the Geant4 simulation to validate
/// Fatras simulation against Geant4. It writes out the particle properties of
/// the first and the last step in order to compare energy loss and displacement
/// due to material effects.

int
main(int argc, char* argv[])
{
  size_t               nEvents   = 10000;
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  FWDD4hep::GeometryService::Config gsConfig("GeometryService",
                                             Acts::Logging::INFO);

  if (argc > 1) {
    std::cout << "Creating detector from xml-file: '" << argv[1] << "'!"
              << std::endl;
    gsConfig.xmlFileName = argv[1];
  } else
    gsConfig.xmlFileName
        = "file:Detectors/DD4hepDetector/compact/DummyDetector.xml";
  gsConfig.bTypePhi                 = Acts::equidistant;
  gsConfig.bTypeR                   = Acts::equidistant;
  gsConfig.bTypeZ                   = Acts::equidistant;
  gsConfig.envelopeR                = 0.;
  gsConfig.envelopeZ                = 0.;
  gsConfig.buildDigitizationModules = false;

  auto geometrySvc = std::make_shared<FWDD4hep::GeometryService>(gsConfig);
  std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
      = geometrySvc->trackingGeometry();

  FWDD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4", Acts::Logging::INFO);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc = std::make_shared<FWDD4hepG4::DD4hepToG4Svc>(dgConfig);

  // set up the particle properties writer
  FWRoot::MaterialEffectsWriter::Config writerConfig;
  writerConfig.treeName = "G4particleProperties";
  writerConfig.fileName = "G4particleProperties.root";
  auto writer = std::make_shared<FWRoot::MaterialEffectsWriter>(writerConfig);

  // The configuration of the PrimaryGeneratorAction
  FWG4::PrimaryGeneratorAction::Config pgaConfig;
  pgaConfig.nParticles   = 1;
  pgaConfig.particleName = "mu-";
  pgaConfig.energy       = 105.;
  pgaConfig.ptRange      = {10. * Acts::units::_GeV, 10. * Acts::units::_GeV};

  // Simulation algorithm
  FWG4::G4FatrasValidation::Config algConfig;
  algConfig.geant4Service         = dd4hepToG4Svc;
  algConfig.materialEffectsWriter = writer;
  algConfig.testsPerEvent         = nEvents;
  algConfig.radialStepLimit       = 1000. * Acts::units::_mm;
  algConfig.pgaConfig             = pgaConfig;
  auto algorithm = std::make_shared<FWG4::G4FatrasValidation>(algConfig);

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  //  sequencer.addServices({geometrySvc});
  //    sequencer.addServices({dd4hepToG4Svc});
  sequencer.addServices({geometrySvc, dd4hepToG4Svc, writer});
  sequencer.appendEventAlgorithms({algorithm});

  // initialize loop
  sequencer.initializeEventLoop();
  // run the loop
  sequencer.processEventLoop(1);
  // finalize loop
  sequencer.finalizeEventLoop();
}
