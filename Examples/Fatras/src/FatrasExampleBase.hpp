#ifndef ACTFW_FATRASEXAMPLE_BASE
#define ACTFW_FATRASEXAMPLE_BASE

#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"  // to be replaced by simulation algorithm
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Fatras/FatrasWriteAlgorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Json/JsonSpacePointWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjExCellWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjSpacePointWriter.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Plugins/Root/TPythia8Generator.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Utilities/Units.hpp"

namespace ACTFWFatrasExample {

int
run(size_t nEvents, std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{
  if (!tGeometry) return -9;

  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
      new Acts::ConstantBField{{0., 0., 2. * Acts::units::_T}});

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FW::initExtrapolator(tGeometry, magField, eLogLevel);

  // create a pythia generator for the hard scatter
  // process: HardQCD for the moment
  FWRoot::TPythia8Generator::Config hsPythiaConfig;
  hsPythiaConfig.pdgBeam0       = 2212;
  hsPythiaConfig.pdgBeam1       = 2212;
  hsPythiaConfig.cmsEnergy      = 14000.;
  hsPythiaConfig.processStrings = {{"HardQCD:all = on"}};
  auto hsPythiaGenerator        = std::make_shared<FWRoot::TPythia8Generator>(
      hsPythiaConfig,
      Acts::getDefaultLogger("HardScatterTPythia8Generator",
                             Acts::Logging::VERBOSE));

  // create a pythia generator for the pile-up
  // MinBias with SD, DD and ND
  FWRoot::TPythia8Generator::Config puPythiaConfig;
  puPythiaConfig.pdgBeam0       = 2212;
  puPythiaConfig.pdgBeam1       = 2212;
  puPythiaConfig.cmsEnergy      = 14000.;
  puPythiaConfig.processStrings = {{"SoftQCD:all = on"}};

  auto puPythiaGenerator = std::make_shared<FWRoot::TPythia8Generator>(
      puPythiaConfig,
      Acts::getDefaultLogger("PileUpTPythia8Generator",
                             Acts::Logging::VERBOSE));

  // random number generation
  // Create the random number engine
  FW::RandomNumbersSvc::Config randomNumbersCfg;
  auto randomNumbers = std::make_shared<FW::RandomNumbersSvc>(randomNumbersCfg);

  // the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", Acts::Logging::INFO));

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

  FW::Csv::CsvParticleWriter::Config pWriterCsvConfig;
  pWriterCsvConfig.outputPrecision = 6;
  pWriterCsvConfig.outputStream    = particleStream;
  pWriterCsvConfig.barcodeSvc      = barcodeSvc;
  auto pWriterCsv
      = std::make_shared<FW::Csv::CsvParticleWriter>(pWriterCsvConfig);

  // ----------- EVGEN --------------------------------------------------
  // get the read-in algorithm
  FW::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.particlesCollection = "EvgenParticles";
  // the hard scatter reader
  readEvgenCfg.hardscatterParticleReader = hsPythiaGenerator;
  // the pileup reader
  readEvgenCfg.pileupParticleReader = puPythiaGenerator;
  // the number of pileup events
  readEvgenCfg.randomNumbers          = randomNumbers;
  readEvgenCfg.pileupPoissonParameter = 200;
  readEvgenCfg.vertexTParameters      = {{0., 0.015 * Acts::units::_mm}};
  readEvgenCfg.vertexZParameters      = {{0., 55.0 * Acts::units::_mm}};
  // attach the barcode service
  readEvgenCfg.barcodeSvc = barcodeSvc;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FW::ReadEvgenAlgorithm>(
      readEvgenCfg,
      Acts::getDefaultLogger("ReadEvgenAlgorithm", Acts::Logging::INFO));

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
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.particlesCollection          = readEvgenCfg.particlesCollection;
  eTestConfig.simulatedParticlesCollection = "FatrasParticles";
  eTestConfig.simulatedHitsCollection      = "FatrasHits";
  eTestConfig.minPt                        = 500. * Acts::units::_MeV;
  eTestConfig.searchMode                   = 1;
  eTestConfig.extrapolationEngine          = extrapolationEngine;
  eTestConfig.ecChargedWriter              = stracksWriterObj;  // rootEcWriter
  eTestConfig.ecNeutralWriter              = nullptr;
  eTestConfig.collectSensitive             = true;
  eTestConfig.collectPassive               = true;
  eTestConfig.collectBoundary              = true;
  eTestConfig.collectMaterial              = true;
  eTestConfig.sensitiveCurvilinear         = false;
  eTestConfig.pathLimit                    = -1.;

  std::shared_ptr<FW::IAlgorithm> extrapolationAlg(
      new FW::ExtrapolationAlgorithm(eTestConfig));

  // ----------- DIGITIZATION ----------------------------------------------
  // set up the planar module stepper
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      pmStepperConfig,
      Acts::getDefaultLogger("PlanarModuleStepper", Acts::Logging::INFO));

  // the configuration ofr the ditigisation algorithm
  FW::DigitizationAlgorithm::Config digConfig;
  digConfig.simulatedHitsCollection = eTestConfig.simulatedHitsCollection;
  digConfig.clustersCollection      = "FatrasClusters";
  digConfig.spacePointsCollection   = "FatrasSpacePoints";
  digConfig.planarModuleStepper     = pmStepper;

  auto digitzationAlg = std::make_shared<FW::DigitizationAlgorithm>(digConfig);

  // ----------- WRITER ----------------------------------------------------
  // write a Csv File for clusters and particles
  auto        hitStream     = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string hitOutputName = "Hits.csv";
  hitStream->open(hitOutputName);

  FW::Csv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
  clusterWriterCsvConfig.collection = digConfig.clustersCollection;
  clusterWriterCsvConfig.outputPrecision = 6;
  clusterWriterCsvConfig.outputStream    = hitStream;
  auto clusterWriterCsv = std::make_shared<FW::Csv::CsvPlanarClusterWriter>(
      clusterWriterCsvConfig);

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
  // the created clusters
  writeConfig.planarClustersCollection = digConfig.clustersCollection;
  // writeConfig.planarClusterWriter      = clusterWriterCsv;
  // the created space points
  writeConfig.spacePointCollection = digConfig.spacePointsCollection;
  writeConfig.spacePointWriter     = spWriterObj;  // spWriterJson;

  auto writeOutput = std::make_shared<FWA::FatrasWriteAlgorithm>(
      writeConfig,
      Acts::getDefaultLogger("FatrasWriteAlgorithm", Acts::Logging::VERBOSE));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({rootEccWriter, stracksWriterObj, randomNumbers});
  sequencer.addReaders({readEvgen});
  sequencer.addWriters({pWriterCsv, clusterWriterCsv, writeOutput});
  sequencer.appendEventAlgorithms({extrapolationAlg, digitzationAlg});
  sequencer.run(nEvents);

  particleStream->close();
  hitStream->close();
  spacePointStream->close();

  // return after successful run
  return 0;
}
};  // namespace ACTFWFatrasExample

#endif  // ACTFW_FATRASEXAMPLE_BASE
