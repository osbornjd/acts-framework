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
#include "Acts/Propagator/AtlasStepper.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Navigator.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

#include "ACTFW/Detector/IBaseDetector.hpp"

namespace {

/// @brief Propgation setup
///
/// @tparam sequencer_t Type of the sequencer of the framework
/// @tparam bfield_t Type of the magnetic field
///
/// @param sequencer The framework sequencer, Propgation algorithm to be added
/// @param bfield The bfield object needed for the Stepper & propagagor
/// @param vm The program options for the log file
/// @param randomNumberSvc The framework random number engine
/// @param tGeometry The TrackingGeometry object
///
/// @return a process code
template <typename sequencer_t, typename bfield_t>
FW::ProcessCode
setupPropagation(sequencer_t&                                  sequencer,
                 bfield_t                                      bfield,
                 boost::program_options::variables_map&        vm,
                 std::shared_ptr<FW::RandomNumbers>            randomNumberSvc,
                 std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{
  // Get the log level
  auto logLevel = FW::Options::readLogLevel(vm);

  // Get a Navigator
  Acts::Navigator navigator(tGeometry);

  // StraightLine Stepper setup
  if (vm["prop-stepper"].template as<int>() == 0) {

    // Straight line stepper
    using SlStepper  = Acts::StraightLineStepper;
    using Propagator = Acts::Propagator<SlStepper, Acts::Navigator>;
    // Make stepper and propagator
    SlStepper  stepper;
    Propagator propagator(std::move(stepper), std::move(navigator));

    // Read the propagation config and create the algorithms
    auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
    pAlgConfig.randomNumberSvc = randomNumberSvc;
    sequencer.addAlgorithm(
        std::make_shared<FW::PropagationAlgorithm<Propagator>>(pAlgConfig,
                                                               logLevel));

  } else if (vm["prop-stepper"].template as<int>() == 1) {
    // Eigen Stepper setup

    // Resolve the bfield map template and create the propgator
    using Stepper    = Acts::EigenStepper<bfield_t>;
    using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;
    Stepper    stepper(std::move(bfield));
    Propagator propagator(std::move(stepper), std::move(navigator));

    // Read the propagation config and create the algorithms
    auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
    pAlgConfig.randomNumberSvc = randomNumberSvc;
    sequencer.addAlgorithm(
        std::make_shared<FW::PropagationAlgorithm<Propagator>>(pAlgConfig,
                                                               logLevel));

  } else if (vm["prop-stepper"].template as<int>() == 2) {
    // ATLAS Stepper setup

    // Resolve the bfield map template and create the propgator
    using Stepper    = Acts::AtlasStepper<bfield_t>;
    using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;
    Stepper    stepper(std::move(bfield));
    Propagator propagator(std::move(stepper), std::move(navigator));

    // Read the propagation config and create the algorithms
    auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
    pAlgConfig.randomNumberSvc = randomNumberSvc;
    sequencer.addAlgorithm(
        std::make_shared<FW::PropagationAlgorithm<Propagator>>(pAlgConfig,
                                                               logLevel));
  }

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
  auto bField  = FW::Options::readBField(vm);
  auto field2D = std::get<std::shared_ptr<InterpolatedBFieldMap2D>>(bField);
  auto field3D = std::get<std::shared_ptr<InterpolatedBFieldMap3D>>(bField);

  if (field2D) {
    // Define the interpolated b-field
    using BField = Acts::SharedBField<InterpolatedBFieldMap2D>;
    BField fieldMap(field2D);
    setupPropagation(sequencer, fieldMap, vm, randomNumberSvc, tGeometry);
  } else if (field3D) {
    // Define the interpolated b-field
    using BField = Acts::SharedBField<InterpolatedBFieldMap3D>;
    BField fieldMap(field3D);
    setupPropagation(sequencer, fieldMap, vm, randomNumberSvc, tGeometry);
  } else if (vm["bf-context-scalable"].template as<bool>()) {
    using SField = FW::BField::ScalableBField;
    SField fieldMap(*std::get<std::shared_ptr<SField>>(bField));
    setupPropagation(sequencer, fieldMap, vm, randomNumberSvc, tGeometry);
  } else {
    // Create the constant  field
    using CField = Acts::ConstantBField;
    CField fieldMap(*std::get<std::shared_ptr<CField>>(bField));
    setupPropagation(sequencer, fieldMap, vm, randomNumberSvc, tGeometry);
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
