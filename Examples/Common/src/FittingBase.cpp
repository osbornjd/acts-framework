#include "detail/FittingBase.hpp"

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/Fitting/FittingAlgorithm.hpp"
#include "ACTFW/Fitting/FittingOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "ACTFW/Plugins/Root/RootPerformanceWriter.hpp"
#include "ACTFW/Plugins/Root/RootTrajectoryWriter.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Fitter/GainMatrixSmoother.hpp"
#include "Acts/Fitter/GainMatrixUpdater.hpp"
#include "Acts/Fitter/KalmanFitter.hpp"
#include "Acts/Geometry/GeometryID.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Surfaces/Surface.hpp"

#include <boost/program_options.hpp>

/// @brief Fitting setup
///
/// @tparam bfield_t Type of the bfield for the fitting to be set up
///
/// @param fieldMap The field map for the fitting setup
/// @param sequencer The framework sequencer
/// @param vm The boost variable map to resolve
/// @param tGeometry The TrackingGeometry for the tracking setup
/// @param barcodesSvc The barcode service to be used for the fitting
/// @param randomNumberSvc The random number service to be used for the
/// fitting
template <typename bfield_t>
void
setupFittingAlgorithm(bfield_t                                      fieldMap,
                      FW::Sequencer&                                sequencer,
                      po::variables_map&                            vm,
                      std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
                      std::shared_ptr<FW::BarcodeSvc>               barcodeSvc,
                      std::shared_ptr<FW::RandomNumbers> randomNumberSvc)
{

  // Read the log level
  Acts::Logging::Level logLevel = FW::Options::readLogLevel(vm);

  // Create a navigator for this tracking geometry
  Acts::Navigator cNavigator(tGeometry);
  cNavigator.resolvePassive   = false;
  cNavigator.resolveMaterial  = true;
  cNavigator.resolveSensitive = true;

  using ChargedStepper    = Acts::EigenStepper<bfield_t>;
  using ChargedPropagator = Acts::Propagator<ChargedStepper, Acts::Navigator>;

  ChargedStepper    cStepper(std::move(fieldMap));
  ChargedPropagator cPropagator(std::move(cStepper), std::move(cNavigator));

  using Updater  = Acts::GainMatrixUpdater<Acts::BoundParameters>;
  using Smoother = Acts::GainMatrixSmoother<Acts::BoundParameters>;

  using KalmanFitter = Acts::KalmanFitter<ChargedPropagator, Updater, Smoother>;

  KalmanFitter kFitter(cPropagator,
                       Acts::getDefaultLogger("KalmanFilter", logLevel));

  using FittingAlgorithm = FW::FittingAlgorithm<KalmanFitter>;

  typename FittingAlgorithm::Config fittingConfig
      = FW::Options::readFittingConfig<po::variables_map, KalmanFitter>(
          vm, std::move(kFitter));
  fittingConfig.randomNumberSvc = randomNumberSvc;

  std::string trackCollection          = fittingConfig.trackCollection;
  std::string simulatedEventCollection = fittingConfig.simulatedEventCollection;

  // Finally the fitting algorithm
  sequencer.addAlgorithm(
      std::make_shared<FittingAlgorithm>(std::move(fittingConfig), logLevel));

  // Output directory
  std::string outputDir = vm["output-dir"].template as<std::string>();

  // Write fitted tracks as ROOT files
  if (vm["output-root"].template as<bool>()) {
    FW::Root::RootTrajectoryWriter::Config tWriterRootConfig;
    tWriterRootConfig.trackCollection          = trackCollection;
    tWriterRootConfig.simulatedEventCollection = simulatedEventCollection;
    tWriterRootConfig.filePath
        = FW::joinPaths(outputDir, trackCollection + ".root");
    tWriterRootConfig.treeName = trackCollection;
    sequencer.addWriter(
        std::make_shared<FW::Root::RootTrajectoryWriter>(tWriterRootConfig));
  }

  // Write performance plots as ROOT files
  if (vm["output-root"].template as<bool>()) {
    FW::ResPlotTool::Config                 resPlotToolConfig;
    FW::Root::RootPerformanceWriter::Config perfValidationConfig;
    perfValidationConfig.resPlotToolConfig        = resPlotToolConfig;
    perfValidationConfig.trackCollection          = trackCollection;
    perfValidationConfig.simulatedEventCollection = simulatedEventCollection;
    perfValidationConfig.filePath
        = FW::joinPaths(outputDir, trackCollection + "_performance.root");
    sequencer.addWriter(std::make_shared<FW::Root::RootPerformanceWriter>(
        perfValidationConfig));
  }
}

void
setupFitting(boost::program_options::variables_map&        vm,
             FW::Sequencer&                                sequencer,
             std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
             std::shared_ptr<FW::BarcodeSvc>               barcodeSvc,
             std::shared_ptr<FW::RandomNumbers>            randomNumberSvc)
{
  // create BField service
  auto bField  = FW::Options::readBField(vm);
  auto field2D = std::get<std::shared_ptr<InterpolatedBFieldMap2D>>(bField);
  auto field3D = std::get<std::shared_ptr<InterpolatedBFieldMap3D>>(bField);

  if (field2D) {
    // Define the interpolated b-field
    using BField = Acts::SharedBField<InterpolatedBFieldMap2D>;
    BField fieldMap(field2D);
    // now setup of the fitting and append it to the sequencer
    setupFittingAlgorithm(std::move(fieldMap),
                          sequencer,
                          vm,
                          tGeometry,
                          barcodeSvc,
                          randomNumberSvc);
  } else if (field3D) {
    // Define the interpolated b-field
    using BField = Acts::SharedBField<InterpolatedBFieldMap3D>;
    BField fieldMap(field3D);
    // now setup of the fitting and append it to the sequencer
    setupFittingAlgorithm(std::move(fieldMap),
                          sequencer,
                          vm,
                          tGeometry,
                          barcodeSvc,
                          randomNumberSvc);
  } else if (vm["bf-context-scalable"].template as<bool>()) {
    using SField = FW::BField::ScalableBField;
    SField fieldMap(*std::get<std::shared_ptr<SField>>(bField));
    // now setup of the fitting and append it to the sequencer
    setupFittingAlgorithm(std::move(fieldMap),
                          sequencer,
                          vm,
                          tGeometry,
                          barcodeSvc,
                          randomNumberSvc);
  } else {
    // Create the constant  field
    using CField = Acts::ConstantBField;
    CField fieldMap(*std::get<std::shared_ptr<CField>>(bField));
    // now setup of the fitting and append it to the sequencer
    setupFittingAlgorithm(std::move(fieldMap),
                          sequencer,
                          vm,
                          tGeometry,
                          barcodeSvc,
                          randomNumberSvc);
  }
}
