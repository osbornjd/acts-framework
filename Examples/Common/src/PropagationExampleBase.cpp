// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <memory>

#include <boost/program_options.hpp>

#include "ACTFW/Framework/RandomNumbers.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Geometry/CommonGeometry.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/BField/ScalableBField.hpp"
#include "ACTFW/Plugins/Obj/ObjPropagationStepsWriter.hpp"
#include "ACTFW/Plugins/Root/RootPropagationStepsWriter.hpp"
#include "ACTFW/Propagation/PropagationAlgorithm.hpp"
#include "ACTFW/Propagation/PropagationOptions.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Geometry/TrackingGeometry.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

#include "ACTFW/Detector/IBaseDetector.hpp"

namespace {

/// @brief Straight Line Propgation setup
///
/// @tparam sequencer_t Type of the sequencer of the framework
///
/// @param sequencer The framework sequencer, Propgation algorithm to be added
/// @param vm The program options for the log file
/// @param randomNumberSvc The framework random number engine
/// @param tGeometry The TrackingGeometry object
///
/// @return a process code
template <typename sequencer_t>
FW::ProcessCode
setupStraightLinePropgation(
    sequencer_t&                                  sequencer,
    boost::program_options::variables_map&        vm,
    std::shared_ptr<FW::RandomNumbers>            randomNumberSvc,
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{
  // Get the log level
  auto logLevel = FW::Options::readLogLevel(vm);

  // Get a Navigator
  Acts::Navigator navigator(tGeometry);

  // Straight line stepper
  using SlStepper  = Acts::StraightLineStepper;
  using Propagator = Acts::Propagator<SlStepper, Acts::Navigator>;
  // Make stepper and propagator
  SlStepper  stepper;
  Propagator propagator(std::move(stepper), std::move(navigator));

  // Read the propagation config and create the algorithms
  auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
  pAlgConfig.randomNumberSvc = randomNumberSvc;
  sequencer.addAlgorithm(std::make_shared<FW::PropagationAlgorithm<Propagator>>(
      pAlgConfig, logLevel));

  return FW::ProcessCode::SUCCESS;
}
}  // namespace

int
propagationExample(int argc, char* argv[], FW::IBaseDetector& detector)
{
  // Setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addGeometryOptions(desc);
  FW::Options::addMaterialOptions(desc);
  FW::Options::addBFieldOptions(desc);
  FW::Options::addRandomNumbersOptions(desc);
  FW::Options::addPropagationOptions(desc);
  FW::Options::addOutputOptions(desc);

  // Add specific options for this geometry
  detector.addOptions(desc);
  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) { return EXIT_FAILURE; }
  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // Now read the standard options
  auto logLevel = FW::Options::readLogLevel(vm);

  // The geometry, material and decoration
  auto geometry          = FW::Geometry::build(vm, detector);
  auto tGeometry         = geometry.first;
  auto contextDecorators = geometry.second;
  // Add the decorator to the sequencer
  for (auto cdr : contextDecorators) { sequencer.addContextDecorator(cdr); }

  // Create the random number engine
  auto randomNumberSvcCfg = FW::Options::readRandomNumbersConfig(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbers>(randomNumberSvcCfg);

  // Create BField service
  auto bFieldVar = FW::Options::readBField(vm);
  // auto field2D = std::get<std::shared_ptr<InterpolatedBFieldMap2D>>(bField);
  // auto field3D = std::get<std::shared_ptr<InterpolatedBFieldMap3D>>(bField);

  if (vm["prop-stepper"].template as<int>() == 0) {
    // Straight line stepper was chosen
    setupStraightLinePropgation(sequencer, vm, randomNumberSvc, tGeometry);
  } else {
    // Get a Navigator
    Acts::Navigator navigator(tGeometry);

    std::visit(
        [&](auto& bField) {
          using field_type =
              typename std::decay_t<decltype(bField)>::element_type;
          Acts::SharedBField<field_type> fieldMap(bField);

          // Resolve the bfield map and create the propgator
          using Stepper    = Acts::EigenStepper<decltype(fieldMap)>;
          using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;
          Stepper    stepper(std::move(fieldMap));
          Propagator propagator(std::move(stepper), std::move(navigator));

          // Read the propagation config and create the algorithms
          auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
          pAlgConfig.randomNumberSvc = randomNumberSvc;
          sequencer.addAlgorithm(
              std::make_shared<FW::PropagationAlgorithm<Propagator>>(pAlgConfig,
                                                                     logLevel));
        },
        bFieldVar);
  }

  // ---------------------------------------------------------------------------------
  // Output directory
  std::string outputDir    = vm["output-dir"].template as<std::string>();
  auto        psCollection = vm["prop-step-collection"].as<std::string>();

  if (vm["output-root"].template as<bool>()) {
    // Write the propagation steps as ROOT TTree
    FW::Root::RootPropagationStepsWriter::Config pstepWriterRootConfig;
    pstepWriterRootConfig.collection = psCollection;
    pstepWriterRootConfig.filePath
        = FW::joinPaths(outputDir, psCollection + ".root");
    sequencer.addWriter(std::make_shared<FW::Root::RootPropagationStepsWriter>(
        pstepWriterRootConfig));
  }

  if (vm["output-obj"].template as<bool>()) {

    using PropagationSteps = Acts::detail::Step;
    using ObjPropagationStepsWriter
        = FW::Obj::ObjPropagationStepsWriter<PropagationSteps>;

    // Write the propagation steps as Obj TTree
    ObjPropagationStepsWriter::Config pstepWriterObjConfig;
    pstepWriterObjConfig.collection = psCollection;
    pstepWriterObjConfig.outputDir  = outputDir;
    sequencer.addWriter(
        std::make_shared<ObjPropagationStepsWriter>(pstepWriterObjConfig));
  }

  return sequencer.run();
}
