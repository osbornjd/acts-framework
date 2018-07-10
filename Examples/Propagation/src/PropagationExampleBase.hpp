// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/StandardOptions.hpp"
#include "ACTFW/Geometry/GeometryOptions.hpp"
#include "ACTFW/Plugins/BField/BFieldOptions.hpp"
#include "ACTFW/Plugins/Root/RootPropagationWriter.hpp"
#include "ACTFW/Propagation/PropagationAlgorithm.hpp"
#include "ACTFW/Propagation/PropagationOptions.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Extrapolator/Navigator.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/MagneticField/InterpolatedBFieldMap.hpp"
#include "Acts/MagneticField/SharedBFieldMap.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Propagator/StraightLineStepper.hpp"

namespace po = boost::program_options;

template <typename sequencer_t, typename bfield_t>
FW::ProcessCode
setupPropgation(sequencer_t&                                  sequencer,
                bfield_t                                      bfield,
                po::variables_map&                            vm,
                std::shared_ptr<FW::RandomNumbersSvc>         randomNumberSvc,
                std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{

  // get the log level
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto logLevel = standardOptions.second;

  // Get a Navigator
  Acts::Navigator navigator(tGeometry);

  // resolve the bfield map template and create the propgator
  using Stepper    = Acts::EigenStepper<bfield_t>;
  using Propagator = Acts::Propagator<Stepper, Acts::Navigator>;

  Stepper    stepper(std::move(bfield));
  Propagator propagator(std::move(stepper), std::move(navigator));

  // read the propagation config and create the algorithms
  auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
  pAlgConfig.randomNumberSvc = randomNumberSvc;

  auto propagationAlg = std::make_shared<FW::PropagationAlgorithm<Propagator>>(
      pAlgConfig, logLevel);

  // add the propagation algorithm
  sequencer.appendEventAlgorithms({propagationAlg});

  return FW::ProcessCode::SUCCESS;
}

template <typename sequencer_t>
FW::ProcessCode
setupStraightLinePropgation(
    sequencer_t&                                  sequencer,
    po::variables_map&                            vm,
    std::shared_ptr<FW::RandomNumbersSvc>         randomNumberSvc,
    std::shared_ptr<const Acts::TrackingGeometry> tGeometry)
{
  // get the log level
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto logLevel = standardOptions.second;

  // Get a Navigator
  Acts::Navigator navigator(tGeometry);
  // Straight line stepper
  using SlStepper  = Acts::StraightLineStepper;
  using Propagator = Acts::Propagator<SlStepper, Acts::Navigator>;
  // Make stepper and propagator
  SlStepper  stepper;
  Propagator propagator(std::move(stepper), std::move(navigator));

  // read the propagation config and create the algorithms
  auto pAlgConfig = FW::Options::readPropagationConfig(vm, propagator);
  pAlgConfig.randomNumberSvc = randomNumberSvc;

  auto propagationAlg = std::make_shared<FW::PropagationAlgorithm<Propagator>>(
      pAlgConfig, logLevel);

  // add the propagation algorithm
  sequencer.appendEventAlgorithms({propagationAlg});

  return FW::ProcessCode::SUCCESS;
}

/// The Extrapolation example
///
/// @tparam geometry_getter_t Type of the geometry getter struct
///
/// @param argc the number of argumetns of the call
/// @param atgv the argument list
/// @param trackingGeometry is the access struct for the trackingGeometry
///
template <typename geometry_options_t, typename geometry_getter_t>
int
propagationExample(int                argc,
                   char*              argv[],
                   geometry_options_t geometryOptions,
                   geometry_getter_t  trackingGeometry)
{

  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // add the standard options
  FW::Options::addStandardOptions<po::options_description>(desc, 1, 2);
  // add the geometry options
  FW::Options::addGeometryOptions<po::options_description>(desc);
  // add the bfield options
  FW::Options::addBFieldOptions<po::options_description>(desc);
  // add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // add the fatras options
  FW::Options::addPropagationOptions<po::options_description>(desc);

  // add specific options for this geometry
  geometryOptions(desc);

  // map to store the given program options
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // now read the standard options
  auto standardOptions
      = FW::Options::readStandardOptions<po::variables_map>(vm);
  auto nEvents  = standardOptions.first;
  auto logLevel = standardOptions.second;

  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);
  // add it to the sequencer
  sequencer.addServices({randomNumberSvc});

  // get the tracking geometry
  auto tGeometry = trackingGeometry(vm);

  // create BField service
  auto bField = FW::Options::readBField<po::variables_map>(vm);

  if (vm["prop-stepper"].template as<int>() == 0) {
    // straight line stepper
    setupStraightLinePropgation(sequencer, vm, randomNumberSvc, tGeometry);
  } else if (bField.first) {
    // define the interpolated b-field
    using BField = Acts::SharedBField<Acts::InterpolatedBFieldMap>;
    BField fieldMap(bField.first);
    setupPropgation(sequencer, fieldMap, vm, randomNumberSvc, tGeometry);
  } else {
    // create the constant  field
    using CField = Acts::ConstantBField;
    CField fieldMap(*bField.second);
    setupPropgation(sequencer, fieldMap, vm, randomNumberSvc, tGeometry);
  }

  if (vm["prop-output"].template as<bool>()) {
    // simulated hits as ROOT TTree
    FW::Root::RootPropagationWriter::Config pstepWriterRootConfig;
    pstepWriterRootConfig.collection = "PropagationSteps";
    pstepWriterRootConfig.filePath   = FW::joinPaths(
        pstepWriterRootConfig.filePath, "propagation-steps.root");
    auto pstepWriterRoot = std::make_shared<FW::Root::RootPropagationWriter>(
        pstepWriterRootConfig);
    if (sequencer.addWriters({pstepWriterRoot}) != FW::ProcessCode::SUCCESS)
      return -1;
  }

  // initiate the run
  sequencer.run(nEvents);
  return 1;
}
