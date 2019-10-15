#include "detail/FatrasDigitizationBase.hpp"

#include "ACTFW/Digitization/DigitizationAlgorithm.hpp"
#include "ACTFW/Digitization/DigitizationOptions.hpp"
#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjSpacePointWriter.hpp"
#include "ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp"
#include "ACTFW/Utilities/OptionsFwd.hpp"
#include "Acts/Plugins/Digitization/PlanarModuleStepper.hpp"

void
setupDigitization(boost::program_options::variables_map& vm,
                  FW::Sequencer&                         sequencer,
                  std::shared_ptr<FW::BarcodeSvc>        barcodeSvc,
                  std::shared_ptr<FW::RandomNumbers>     randomNumberSvc)
{
  // Read the standard options
  auto logLevel = FW::Options::readLogLevel(vm);

  // Set the module stepper
  auto pmStepper = std::make_shared<Acts::PlanarModuleStepper>(
      Acts::getDefaultLogger("PlanarModuleStepper", logLevel));

  // Read the digitization configuration
  auto digiConfig = FW::Options::readDigitizationConfig(vm);
  // Set the random number service
  digiConfig.randomNumberSvc     = randomNumberSvc;
  digiConfig.planarModuleStepper = pmStepper;

  // set te hit collection
  digiConfig.simulatedHitCollection
      = vm["fatras-sim-hits"].template as<std::string>();

  // Create the algorithm and add it to the sequencer
  sequencer.addAlgorithm(
      std::make_shared<FW::DigitizationAlgorithm>(digiConfig, logLevel));

  // Output directory
  std::string outputDir = vm["output-dir"].template as<std::string>();

  // Write digitisation output as Csv files
  if (vm["output-csv"].template as<bool>()) {
    // clusters as root
    FW::Csv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
    clusterWriterCsvConfig.inputClusters = digiConfig.clusterCollection;
    clusterWriterCsvConfig.outputDir     = outputDir;
    auto clusteWriterCsv = std::make_shared<FW::Csv::CsvPlanarClusterWriter>(
        clusterWriterCsvConfig);
    // Add to the sequencer
    sequencer.addWriter(clusteWriterCsv);
  }

  // Write digitsation output as ROOT files
  if (vm["output-root"].template as<bool>()) {
    // clusters as root
    FW::Root::RootPlanarClusterWriter::Config clusterWriterRootConfig;
    clusterWriterRootConfig.collection = digiConfig.clusterCollection;
    clusterWriterRootConfig.filePath
        = FW::joinPaths(outputDir, digiConfig.clusterCollection + ".root");
    clusterWriterRootConfig.treeName = digiConfig.clusterCollection;
    auto clusteWriterRoot = std::make_shared<FW::Root::RootPlanarClusterWriter>(
        clusterWriterRootConfig);
    // Add to the sequencer
    sequencer.addWriter(clusteWriterRoot);
  }
}
