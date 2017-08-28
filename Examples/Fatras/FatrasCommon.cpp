#include "FatrasCommon.hpp"

#include <ACTS/Digitization/PlanarModuleStepper.hpp>
#include <ACTS/MagneticField/ConstantBField.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationAlgorithm.hpp"
#include "ACTFW/Extrapolation/ExtrapolationUtils.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Json/JsonSpacePointWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjSpacePointWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp"

FW::ProcessCode
setupSimulation(FW::Sequencer&                                sequencer,
                std::shared_ptr<const Acts::TrackingGeometry> geometry,
                std::shared_ptr<FW::RandomNumbersSvc>         random,
                Acts::Logging::Level defaultLevel)
{
  
  // magnetic field
  auto bfield
      = std::make_shared<Acts::ConstantBField>(0., 0., 2. * Acts::units::_T);

  // extrapolation algorithm
  FW::ExtrapolationAlgorithm::Config eTestConfig;
  eTestConfig.evgenCollection              = "EvgenParticles";
  eTestConfig.simulatedParticlesCollection = "FatrasParticles";
  eTestConfig.simulatedHitsCollection      = "FatrasHits";
  eTestConfig.searchMode                   = 1;
  eTestConfig.extrapolationEngine
      = FW::initExtrapolator(geometry, bfield, defaultLevel);
  eTestConfig.skipNeutral          = true; 
  eTestConfig.collectSensitive     = true;
  eTestConfig.collectPassive       = true;
  eTestConfig.collectBoundary      = true;
  eTestConfig.collectMaterial      = true;
  eTestConfig.sensitiveCurvilinear = false;
  eTestConfig.pathLimit            = -1.;
  auto extrapolationAlg
    = std::make_shared<FW::ExtrapolationAlgorithm>(eTestConfig, defaultLevel);

  // digitisation
  Acts::PlanarModuleStepper::Config pmStepperConfig;
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      pmStepperConfig,
      Acts::getDefaultLogger("PlanarModuleStepper", defaultLevel));

  FW::DigitizationAlgorithm::Config digConfig;
  digConfig.simulatedHitsCollection = eTestConfig.simulatedHitsCollection;
  digConfig.clustersCollection      = "FatrasClusters";
  digConfig.spacePointsCollection   = "FatrasSpacePoints";
  digConfig.planarModuleStepper     = pmStepper;
  auto digitzationAlg 
    = std::make_shared<FW::DigitizationAlgorithm>(digConfig, defaultLevel);

  // add algorithms to sequencer
  if (sequencer.appendEventAlgorithms({extrapolationAlg, digitzationAlg})
      != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}

FW::ProcessCode
setupWriters(FW::Sequencer&                  sequencer,
             std::shared_ptr<FW::BarcodeSvc> barcode,
             std::string                     outputDir,
             Acts::Logging::Level defaultLevel)
{
  const std::string particles = "FatrasParticles";
  const std::string clusters  = "FatrasClusters";
  const std::string points    = "FatrasSpacePoints";

  // particles as csv
  FW::Csv::CsvParticleWriter::Config pWriterCsvConfig;
  pWriterCsvConfig.collection = particles;
  pWriterCsvConfig.outputDir  = outputDir;
  pWriterCsvConfig.barcodeSvc = barcode;
  auto pWriterCsv
      = std::make_shared<FW::Csv::CsvParticleWriter>(pWriterCsvConfig);

  // particles as ROOT TTree
  FW::Root::RootParticleWriter::Config pWriterRootConfig;
  pWriterRootConfig.collection = particles;
  pWriterRootConfig.filePath   = FW::joinPaths(outputDir, "particles.root");
  pWriterRootConfig.barcodeSvc = barcode;
  auto pWriterRoot
      = std::make_shared<FW::Root::RootParticleWriter>(pWriterRootConfig);

  // clusters as csv
  FW::Csv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
  clusterWriterCsvConfig.collection = clusters;
  clusterWriterCsvConfig.outputDir  = outputDir;
  auto clusterWriterCsv = std::make_shared<FW::Csv::CsvPlanarClusterWriter>(
      clusterWriterCsvConfig);

  // clusters as root
  FW::Root::RootPlanarClusterWriter::Config clusterWriterRootConfig;
  clusterWriterRootConfig.collection = clusters;
  clusterWriterRootConfig.filePath = FW::joinPaths(outputDir, "clusters.root");
  auto clusteWriterRoot = std::make_shared<FW::Root::RootPlanarClusterWriter>(
    clusterWriterRootConfig);

  // space points as json
  FW::Json::JsonSpacePointWriter<Acts::Vector3D>::Config spWriterJsonConfig;
  spWriterJsonConfig.collection = points;
  spWriterJsonConfig.outputDir  = outputDir;
  auto spWriterJson
      = std::make_shared<FW::Json::JsonSpacePointWriter<Acts::Vector3D>>(
          spWriterJsonConfig);

  // space points as obj
  FW::Obj::ObjSpacePointWriter<Acts::Vector3D>::Config spWriterObjConfig;
  spWriterObjConfig.collection = points;
  spWriterObjConfig.outputDir  = outputDir;
  auto spWriterObj
      = std::make_shared<FW::Obj::ObjSpacePointWriter<Acts::Vector3D>>(
          spWriterObjConfig);

  // add to sequencer
  if (sequencer.addWriters({pWriterCsv,
                            pWriterRoot,
                            clusterWriterCsv,
                            clusteWriterRoot,
                            spWriterJson,
                            spWriterObj})
      != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
