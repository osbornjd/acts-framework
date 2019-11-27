// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

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

namespace po = boost::program_options;

void
setupFitting(boost::program_options::variables_map&        vm,
             FW::Sequencer&                                sequencer,
             std::shared_ptr<const Acts::TrackingGeometry> tGeometry,
             std::shared_ptr<FW::BarcodeSvc>               barcodeSvc,
             std::shared_ptr<FW::RandomNumbers>            randomNumberSvc)
{

  using Updater  = Acts::GainMatrixUpdater<Acts::BoundParameters>;
  using Smoother = Acts::GainMatrixSmoother<Acts::BoundParameters>;

  // Read the log level
  Acts::Logging::Level logLevel = FW::Options::readLogLevel(vm);

  // Create a navigator for this tracking geometry
  Acts::Navigator cNavigator(tGeometry);
  cNavigator.resolvePassive   = false;
  cNavigator.resolveMaterial  = true;
  cNavigator.resolveSensitive = true;

  FW::FittingAlgorithm::Config fittingConfig;
  FW::Options::readFittingConfig(vm, fittingConfig);
  fittingConfig.randomNumberSvc = randomNumberSvc;

  // create BField service
  auto bFieldVar = FW::Options::readBField(vm);
  std::visit(
      [&](auto& bField) {
        using field_type =
            typename std::decay_t<decltype(bField)>::element_type;
        Acts::SharedBField<field_type> fieldMap(bField);

        using ChargedStepper = Acts::EigenStepper<decltype(fieldMap)>;
        using ChargedPropagator
            = Acts::Propagator<ChargedStepper, Acts::Navigator>;

        ChargedStepper    cStepper(std::move(fieldMap));
        ChargedPropagator cPropagator(std::move(cStepper),
                                      std::move(cNavigator));

        using KalmanFitter
            = Acts::KalmanFitter<ChargedPropagator, Updater, Smoother>;

        auto kFitter = std::make_shared<KalmanFitter>(
            cPropagator, Acts::getDefaultLogger("KalmanFilter", logLevel));

        fittingConfig.fitFunction
            = [kFitter](
                  std::vector<FW::FittingAlgorithm::Identifier>& sourceLinks,
                  const FW::FittingAlgorithm::StartParameters&   sParameters,
                  const Acts::KalmanFitterOptions&               kfOptions)
            -> FW::FittingAlgorithm::ResultType {
          // TODO propagate error using the result type
          return kFitter->fit(sourceLinks, sParameters, kfOptions).value();
        };
      },
      bFieldVar);

  std::string trackCollection          = fittingConfig.trackCollection;
  std::string simulatedEventCollection = fittingConfig.simulatedEventCollection;

  // Finally the fitting algorithm
  sequencer.addAlgorithm(std::make_shared<FW::FittingAlgorithm>(
      std::move(fittingConfig), logLevel));

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
