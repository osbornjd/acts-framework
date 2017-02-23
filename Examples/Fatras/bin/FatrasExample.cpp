//
//  ReadEvgenExample.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Digitization/PlanarModuleStepper.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Root/RootExCellWriter.hpp"
#include "ACTFW/Root/ParticlePropertiesWriter.hpp"
#include "ACTFW/RootPythia8/ParticleGenerator.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationTestAlgorithm.hpp"  // to be replaced by simulation algorithm
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Fatras/FatrasWriteAlgorithm.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 10;

  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel = Acts::Logging::INFO;
  Acts::Logging::Level layerLogLevel   = Acts::Logging::INFO;
  Acts::Logging::Level volumeLogLevel  = Acts::Logging::INFO;

  // create the tracking geometry as a shared pointer
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = Acts::buildGenericDetector(
          surfaceLogLevel, layerLogLevel, volumeLogLevel, 3);

  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;

  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
      new Acts::ConstantBField{{0., 0., 0.002}});  // * Acts::units::_T

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
      = FWE::initExtrapolator(tGeometry, magField, eLogLevel);

  // creating the data stores
  auto detectorStore = std::make_shared<FW::WhiteBoard>(
      Acts::getDefaultLogger("DetectorStore", Acts::Logging::INFO));

  // create a pythia generator for the hard scatter
  // process: HardQCD for the moment
  FWRootPythia8::ParticleGenerator::Config hsPythiaConfig;
  hsPythiaConfig.pdgBeam0       = 2212;
  hsPythiaConfig.pdgBeam1       = 2212;
  hsPythiaConfig.cmsEnergy      = 14000.;
  hsPythiaConfig.processStrings = {{"HardQCD:all = on"}};
  auto hsPythiaGenerator = std::make_shared<FWRootPythia8::ParticleGenerator>(
      hsPythiaConfig,
      Acts::getDefaultLogger("HardScatterParticleGenerator",
                             Acts::Logging::VERBOSE));

  // create a pythia generator for the pile-up
  // MinBias with SD, DD and ND
  FWRootPythia8::ParticleGenerator::Config puPythiaConfig;
  puPythiaConfig.pdgBeam0       = 2212;
  puPythiaConfig.pdgBeam1       = 2212;
  puPythiaConfig.cmsEnergy      = 14000.;
  puPythiaConfig.processStrings = {{"SoftQCD:all = on"}};

  auto puPythiaGenerator = std::make_shared<FWRootPythia8::ParticleGenerator>(
      puPythiaConfig,
      Acts::getDefaultLogger("PileUpParticleGenerator",
                             Acts::Logging::VERBOSE));

  // random number generation
  // Create the random number engine
  FW::RandomNumbersSvc::Config pileupNumbersCfg;
  pileupNumbersCfg.poisson_parameter = 200;
  std::shared_ptr<FW::RandomNumbersSvc> pileupNumbers(
      new FW::RandomNumbersSvc(pileupNumbersCfg));

  FW::RandomNumbersSvc::Config pileupVertexTCfg;
  pileupVertexTCfg.gauss_parameters = {{0., 0.015 * Acts::units::_mm}};
  std::shared_ptr<FW::RandomNumbersSvc> pileupVertexT(
      new FW::RandomNumbersSvc(pileupVertexTCfg));

  FW::RandomNumbersSvc::Config pileupVertexZCfg;
  pileupVertexZCfg.gauss_parameters = {{0., 55.0 * Acts::units::_mm}};
  std::shared_ptr<FW::RandomNumbersSvc> pileupVertexZ(
      new FW::RandomNumbersSvc(pileupVertexZCfg));

  // the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", Acts::Logging::INFO));

  // Write ROOT TTree
  FWRoot::ParticlePropertiesWriter::Config pWriterRootConfig;
  pWriterRootConfig.fileName   = "$PWD/Evgen.root";
  pWriterRootConfig.treeName   = "Evgen";
  pWriterRootConfig.barcodeSvc = barcodeSvc;
  auto pWriterRoot
      = std::make_shared<FWRoot::ParticlePropertiesWriter>(pWriterRootConfig);
  // or write a Csv File
  auto        particleStream = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string particleOutputName   = "Particles.csv";
  particleStream->open(particleOutputName);

  FWCsv::CsvParticleWriter::Config pWriterCsvConfig;
  pWriterCsvConfig.outputPrecision = 6;
  pWriterCsvConfig.outputStream    = particleStream;
  pWriterCsvConfig.barcodeSvc      = barcodeSvc;
  auto pWriterCsv
      = std::make_shared<FWCsv::CsvParticleWriter>(pWriterCsvConfig);

  std::shared_ptr<FW::IParticlePropertiesWriter> pWriter = pWriterCsv;

  // ----------- EVGEN --------------------------------------------------
  // get the read-in algorithm
  FWE::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.evgenParticlesCollection = "EvgenParticles";
  // the hard scatter reader
  readEvgenCfg.hardscatterParticleReader = hsPythiaGenerator;
  // the pileup reader
  readEvgenCfg.pileupParticleReader = puPythiaGenerator;
  // the number of pileup events
  readEvgenCfg.pileupType          = FW::Distribution::poisson;
  readEvgenCfg.pileupRandomNumbers = pileupNumbers;
  readEvgenCfg.pileupVertexDistT   = pileupVertexT;
  readEvgenCfg.pileupVertexDistZ   = pileupVertexZ;
  // attach the barcode service
  readEvgenCfg.barcodeSvc = barcodeSvc;
  // the job WhiteBoard
  readEvgenCfg.jBoard = detectorStore;
  // set the particle writer
  readEvgenCfg.particleWriter = nullptr;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FWE::ReadEvgenAlgorithm>(
      readEvgenCfg,
      Acts::getDefaultLogger("ReadEvgenAlgorithm", Acts::Logging::INFO));

  // ----------- EXTRAPOLATION ----------------------------------------------
  // Write ROOT TTree
  FWRoot::RootExCellWriter::Config recWriterConfig;
  recWriterConfig.fileName            = "$PWD/ExtrapolationTest.root";
  recWriterConfig.treeName            = "ExtrapolationTest";
  recWriterConfig.writeBoundary       = false;
  recWriterConfig.writeMaterial       = false;
  recWriterConfig.writeSensitive      = true;
  recWriterConfig.writePassive        = false;
  std::shared_ptr<FW::IExtrapolationCellWriter> rootEcWriter(
      new FWRoot::RootExCellWriter(recWriterConfig));
  // the Algorithm with its configurations
  FWE::ExtrapolationTestAlgorithm::Config eTestConfig;
  eTestConfig.evgenParticlesCollection = readEvgenCfg.evgenParticlesCollection;
  eTestConfig.simulatedParticlesCollection = "FatrasParticles";
  eTestConfig.simulatedHitsCollection      = "FatrasHits";
  eTestConfig.minPt                        = 500.;
  eTestConfig.searchMode                   = 1;
  eTestConfig.extrapolationEngine          = extrapolationEngine;
  eTestConfig.extrapolationCellWriter      = rootEcWriter;
  eTestConfig.collectSensitive             = true;
  eTestConfig.collectPassive               = true;
  eTestConfig.collectBoundary              = true;
  eTestConfig.collectMaterial              = true;
  eTestConfig.sensitiveCurvilinear         = false;
  eTestConfig.pathLimit                    = -1.;

  std::shared_ptr<FW::IAlgorithm> extrapolationAlg(
      new FWE::ExtrapolationTestAlgorithm(eTestConfig));

  // ----------- DIGITIZATION ----------------------------------------------
  // set up the planar module stepper
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(pmStepperConfig,
  Acts::getDefaultLogger("PlanarModuleStepper", Acts::Logging::INFO));
  
  // the configuration ofr the ditigisation algorithm
  FWE::DigitizationAlgorithm::Config digConfig;
  digConfig.simulatedHitsCollection = eTestConfig.simulatedHitsCollection;
  digConfig.clustersCollection      = "FatrasClusters";
  digConfig.planarModuleStepper     = pmStepper;

  std::shared_ptr<FW::IAlgorithm> digitzationAlg(new FWE::DigitizationAlgorithm(
      digConfig,
      Acts::getDefaultLogger("DigitizationAlgorithm", Acts::Logging::INFO)));

  // ----------- WRITER ----------------------------------------------------
  // or write a Csv File
  auto        hitStream = std::shared_ptr<std::ofstream>(new std::ofstream);
  std::string hitOutputName   = "Hits.csv";
  hitStream->open(hitOutputName);
      
  FWCsv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
  clusterWriterCsvConfig.outputPrecision = 6;
  clusterWriterCsvConfig.outputStream    = hitStream;
  auto clusterWriterCsv
      = std::make_shared<FWCsv::CsvPlanarClusterWriter>(clusterWriterCsvConfig);
      
      
  FWE::FatrasWriteAlgorithm::Config writeConfig;
  // the simulated particles
  writeConfig.simulatedParticlesCollection
      = eTestConfig.simulatedParticlesCollection;
  writeConfig.particleWriter = pWriter;
  /// the created clusters
  writeConfig.planarClustersCollection = digConfig.clustersCollection;
  writeConfig.planarClusterWriter      = clusterWriterCsv;

  auto writeOutput = std::make_shared<FWE::FatrasWriteAlgorithm>(
      writeConfig,
      Acts::getDefaultLogger("ReadEvgenAlgorithm", Acts::Logging::INFO));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({rootEcWriter, pWriter, pileupNumbers, pileupVertexT, pileupVertexZ});
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
  
}
