//
//  ReadEvgenExample.cpp
//  ACTFW
//
//  Created by Andreas Salzburger on 11/05/16.
//

#include <memory>
#include "ACTS/Examples/BuildGenericDetector.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTFW/Framework/Algorithm.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Root/RootExCellWriter.hpp"
#include "ACTFW/Root/ParticlePropertiesWriter.hpp"
#include "ACTFW/RootPythia8/ParticleGenerator.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationTestAlgorithm.hpp" // to be replaced by simulation algorithm
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"

// the main hello world executable
int
main(int argc, char* argv[])
{
  size_t nEvents = 10;

  // set geometry building logging level
  Acts::Logging::Level surfaceLogLevel = Acts::Logging::INFO;
  Acts::Logging::Level layerLogLevel = Acts::Logging::INFO;
  Acts::Logging::Level volumeLogLevel = Acts::Logging::INFO;
  
  // create the tracking geometry as a shared pointer
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry
    = Acts::buildGenericDetector(surfaceLogLevel,
                                 layerLogLevel,
                                 volumeLogLevel, 3);

  // set extrapolation logging level
  Acts::Logging::Level eLogLevel = Acts::Logging::INFO;
  
  // set up the magnetic field
  std::shared_ptr<Acts::ConstantBField> magField(
      new Acts::ConstantBField{{0., 0., 0.002 }}); // * Acts::units::_T

  // EXTRAPOLATOR - set up the extrapolator
  std::shared_ptr<Acts::IExtrapolationEngine> extrapolationEngine
     = FWE::initExtrapolator(tGeometry,magField,eLogLevel);

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
  auto barcodeSvc = std::make_shared<FW::BarcodeSvc>(
       barcodeSvcCfg,
       Acts::getDefaultLogger("BarcodeSvc", Acts::Logging::INFO));

  // Write ROOT TTree
  FWRoot::ParticlePropertiesWriter::Config particleWriterConfig;
  particleWriterConfig.fileName = "$PWD/Evgen.root";
  particleWriterConfig.treeName = "Evgen";
  particleWriterConfig.barcodeSvc = barcodeSvc;
  auto particleWriter = std::make_shared<FWRoot::ParticlePropertiesWriter>(
      particleWriterConfig);

  // get the read-in algorithm
  FWE::ReadEvgenAlgorithm::Config readEvgenCfg;

  readEvgenCfg.particleCollectionName = "SimulationParticles";
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
  readEvgenCfg.barcodeSvc          = barcodeSvc;
  // the job WhiteBoard
  readEvgenCfg.jBoard = detectorStore;
  // set the particle writer
  readEvgenCfg.particleWriter = particleWriter;
  // create the read Algorithm
  auto readEvgen = std::make_shared<FWE::ReadEvgenAlgorithm>(
      readEvgenCfg, Acts::getDefaultLogger("ReadEvgenAlgorithm", Acts::Logging::INFO));
  
  // Write ROOT TTree
  FWRoot::RootExCellWriter::Config recWriterConfig;
  recWriterConfig.fileName            = "$PWD/Fatras.root";
  recWriterConfig.treeName            = "Fatras";
  recWriterConfig.writeBoundary       = false;
  recWriterConfig.writeMaterial       = false;
  recWriterConfig.writeSensitive      = true;
  recWriterConfig.writePassive        = false;
  std::shared_ptr<FW::IExtrapolationCellWriter> rootEcWriter(
      new FWRoot::RootExCellWriter(recWriterConfig));
  
  // the Algorithm with its configurations
  FWE::ExtrapolationTestAlgorithm::Config eTestConfig;
  eTestConfig.particleCollectionName = readEvgenCfg.particleCollectionName;
  eTestConfig.searchMode              = 1;
  eTestConfig.extrapolationEngine     = extrapolationEngine;
  eTestConfig.extrapolationCellWriter = rootEcWriter;
  eTestConfig.collectSensitive        = true;
  eTestConfig.collectPassive          = true;
  eTestConfig.collectBoundary         = true;
  eTestConfig.collectMaterial         = true;
  eTestConfig.sensitiveCurvilinear    = false;
  eTestConfig.pathLimit               = -1.;
  
  std::shared_ptr<FW::IAlgorithm> extrapolationAlg(
      new FWE::ExtrapolationTestAlgorithm(eTestConfig));

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices(
      {rootEcWriter, particleWriter, pileupNumbers, pileupVertexT, pileupVertexZ});
  sequencer.addIOAlgorithms({readEvgen});
  sequencer.appendEventAlgorithms({extrapolationAlg});

  // initialize loop
  sequencer.initializeEventLoop();
  // run the loop
  sequencer.processEventLoop(nEvents);
  // finalize loop
  sequencer.finalizeEventLoop();

}
