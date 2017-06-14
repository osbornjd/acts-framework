#ifndef ACTFW_FATRASEXAMPLE_BASE
#define ACTFW_FATRASEXAMPLE_BASE
//
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"  // to be replaced by simulation algorithm
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Fatras/FatrasWriteAlgorithm.hpp"
#include "ACTFW/Fatras/ParticleGun.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Json/JsonSpacePointWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjExCellWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjSpacePointWriter.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Utilities/Units.hpp"

namespace ACTFWFatrasPGExample {

int
run(size_t nEvents, std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{
  if (!tGeometry) return -9;

  // set the geometry logging level
  Acts::Logging::Level gLogLevel = Acts::Logging::INFO;
  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;
  // set the fatras logging level
  Acts::Logging::Level fLogLevel = Acts::Logging::INFO;
  // set the digitization logging level
  Acts::Logging::Level dLogLevel = Acts::Logging::INFO;

  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
      new Acts::ConstantBField{{0., 0., 0.002}});  // * Acts::units::_T

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FWA::initExtrapolator(tGeometry, magField, eLogLevel);

  // creating the data stores
  auto detectorStore = std::make_shared<FW::WhiteBoard>(
      Acts::getDefaultLogger("DetectorStore", gLogLevel));

  // RANDOM NUMBERS - Create the random number engine
  FW::RandomNumbersSvc::Config brConfig;
  brConfig.seed = 1234567890;
  std::shared_ptr<FW::RandomNumbersSvc> randomNumbers(
      new FW::RandomNumbersSvc(brConfig));

  // set up the particle gun
  FWE::ParticleGun::Config particleGunConfig;
  particleGunConfig.randomNumbers = randomNumbers;
  particleGunConfig.nParticles    = 100;
  particleGunConfig.etaRange      = {{-5., 5.}};
  particleGunConfig.phiRange      = {{-M_PI, M_PI}};
  particleGunConfig.ptRange       = {{100., 100000.}};
  particleGunConfig.mass          = 105. * Acts::units::_MeV;
  particleGunConfig.charge        = -1. * Acts::units::_e;
  particleGunConfig.pID           = 13;
  auto particleGun                = std::make_shared<FWE::ParticleGun>(
      particleGunConfig, Acts::getDefaultLogger("ParticleGun", fLogLevel));

  // the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", fLogLevel));

  // Write ROOT TTree
  FWRoot::RootParticleWriter::Config pWriterRootConfig;
  pWriterRootConfig.fileName   = "$PWD/Evgen.root";
  pWriterRootConfig.treeName   = "Evgen";
  pWriterRootConfig.barcodeSvc = barcodeSvc;
  auto pWriterRoot
      = std::make_shared<FWRoot::RootParticleWriter>(pWriterRootConfig);
  // or write a Csv File
  auto particleStream = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string particleOutputName = "Particles.csv";
  particleStream->open(particleOutputName);

  FWCsv::CsvParticleWriter::Config pWriterCsvConfig;
  pWriterCsvConfig.outputPrecision = 6;
  pWriterCsvConfig.outputStream    = particleStream;
  pWriterCsvConfig.barcodeSvc      = barcodeSvc;
  auto pWriterCsv
      = std::make_shared<FWCsv::CsvParticleWriter>(pWriterCsvConfig);

  // ----------- EVGEN --------------------------------------------------
  // get the read-in algorithm
  FWA::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.evgenParticlesCollection = "ParticleGun";
  // the hard scatter reader
  readEvgenCfg.hardscatterParticleReader = particleGun;
  // The random number service
  readEvgenCfg.randomNumbers = randomNumbers;
  // attach the barcode service
  readEvgenCfg.barcodeSvc = barcodeSvc;
  // the job WhiteBoard
  readEvgenCfg.jBoard = detectorStore;
  // set the particle writer
  readEvgenCfg.particleWriter = nullptr;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FWA::ReadEvgenAlgorithm>(
      readEvgenCfg, Acts::getDefaultLogger("ReadEvgenAlgorithm", fLogLevel));

  // ----------- EXTRAPOLATION ----------------------------------------------
  // Write ROOT TTree
  // ecc for charged particles
  FWRoot::RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.fileName       = "$PWD/ExtrapolationChargedTest.root";
  reccWriterConfig.treeName       = "ExtrapolationChargedTest";
  reccWriterConfig.writeBoundary  = false;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters>>>
      rootEccWriter(new FWRoot::RootExCellWriter<Acts::TrackParameters>(
          reccWriterConfig));

  // Write OBj file
  auto        stracksStream = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string stracksOutputName = "SimulatedTracks.obj";
  stracksStream->open(stracksOutputName);

  FWObj::ObjExCellWriter<Acts::TrackParameters>::Config stracksWriterObjConfig;
  stracksWriterObjConfig.outputPrecision = 4;
  stracksWriterObjConfig.outputStream    = stracksStream;
  std::shared_ptr<FW::IWriterT<Acts::ExtrapolationCell<Acts::TrackParameters>>>
      stracksWriterObj(new FWObj::ObjExCellWriter<Acts::TrackParameters>(
          stracksWriterObjConfig));

  // the Algorithm with its configurations
  FWA::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.evgenParticlesCollection = readEvgenCfg.evgenParticlesCollection;
  eTestConfig.simulatedParticlesCollection = "FatrasParticles";
  eTestConfig.simulatedHitsCollection      = "FatrasHits";
  eTestConfig.minPt                        = 0.;
  eTestConfig.maxEta                       = 6.;
  eTestConfig.searchMode                   = 1;
  eTestConfig.extrapolationEngine          = extrapolationEngine;
  eTestConfig.ecChargedWriter      = rootEccWriter;  //  stracksWriterObj
  eTestConfig.ecNeutralWriter      = nullptr;
  eTestConfig.collectSensitive     = true;
  eTestConfig.collectPassive       = true;
  eTestConfig.collectBoundary      = true;
  eTestConfig.collectMaterial      = true;
  eTestConfig.sensitiveCurvilinear = false;
  eTestConfig.pathLimit            = -1.;
  //  eTestConfig.particleType                 = Acts::ParticleType::muon;

  std::shared_ptr<FW::IAlgorithm> extrapolationAlg(
      new FWA::ExtrapolationAlgorithm(eTestConfig));

  // ----------- DIGITIZATION ----------------------------------------------
  // set up the planar module stepper
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      pmStepperConfig,
      Acts::getDefaultLogger("PlanarModuleStepper", dLogLevel));

  // the configuration ofr the ditigisation algorithm
  FWA::DigitizationAlgorithm::Config digConfig;
  digConfig.simulatedHitsCollection = eTestConfig.simulatedHitsCollection;
  digConfig.clustersCollection      = "FatrasClusters";
  digConfig.spacePointCollection    = "FatrasSpacePoints";
  digConfig.planarModuleStepper     = pmStepper;

  std::shared_ptr<FW::IAlgorithm> digitzationAlg(new FWA::DigitizationAlgorithm(
      digConfig, Acts::getDefaultLogger("DigitizationAlgorithm", dLogLevel)));

  // ----------- WRITER ----------------------------------------------------
  // write a Csv File for clusters and particles
  auto        hitStream     = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string hitOutputName = "Hits.csv";
  hitStream->open(hitOutputName);

  FWCsv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
  clusterWriterCsvConfig.outputPrecision = 6;
  clusterWriterCsvConfig.outputStream    = hitStream;
  auto clusterWriterCsv
      = std::make_shared<FWCsv::CsvPlanarClusterWriter>(clusterWriterCsvConfig);

  // write out a Json file for
  auto spacePointStream    = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string spOutputName = "SpacePoints.obj";
  spacePointStream->open(spOutputName);

  // FWJson::JsonSpacePointWriter<Acts::Vector3D>::Config spWriterJsonConfig;
  // spWriterJsonConfig.outputPrecision = 6;
  // spWriterJsonConfig.outputStream    = spacePointStream;
  // auto spWriterJson
  //    =
  //    std::make_shared<FWJson::JsonSpacePointWriter<Acts::Vector3D>>(spWriterJsonConfig);

  FWObj::ObjSpacePointWriter<Acts::Vector3D>::Config spWriterObjConfig;
  spWriterObjConfig.outputPrecision = 6;
  spWriterObjConfig.outputStream    = spacePointStream;
  auto spWriterObj
      = std::make_shared<FWObj::ObjSpacePointWriter<Acts::Vector3D>>(
          spWriterObjConfig);

  FWA::FatrasWriteAlgorithm::Config writeConfig;
  // the simulated particles
  writeConfig.simulatedParticlesCollection
      = eTestConfig.simulatedParticlesCollection;
  writeConfig.particleWriter = pWriterCsv;
  // the created clusters
  writeConfig.planarClustersCollection = digConfig.clustersCollection;
  writeConfig.planarClusterWriter      = clusterWriterCsv;
  // the created space points
  writeConfig.spacePointCollection = digConfig.spacePointCollection;
  writeConfig.spacePointWriter     = spWriterObj;  // spWriterJson;

  auto writeOutput = std::make_shared<FWA::FatrasWriteAlgorithm>(
      writeConfig, Acts::getDefaultLogger("FatrasWriteAlgorithm", fLogLevel));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({rootEccWriter, stracksWriterObj});
  sequencer.addIOAlgorithms({readEvgen, writeOutput});
  sequencer.appendEventAlgorithms({extrapolationAlg, digitzationAlg});

  // initialize loop
  sequencer.initializeEventLoop();
  // run the loop
  sequencer.processEventLoop(nEvents);
  // finalize loop
  sequencer.finalizeEventLoop();

  particleStream->close();
  hitStream->close();
  spacePointStream->close();

  // return after successful run
  return 0;
}
};

#endif  // ACTFW_FATRASEXAMPLE_BASE
