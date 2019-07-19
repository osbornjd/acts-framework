// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <memory>

#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Vertexing/FullBilloirVertexFitter.hpp"

#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Options/Pythia8Options.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Utilities/Paths.hpp"

#include "VertexFitAlgorithm.hpp"

#include "ACTFW/Simulation/EventToTrackConverter.hpp"
#include "ACTFW/Simulation/EventToTrackConverterOptions.hpp"
#include "ACTFW/Simulation/TrackSelector.hpp"

using namespace FW;

/// Main vertex fitter example executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
  // setup and parse options
  auto desc = Options::makeDefaultOptions();
  Options::addSequencerOptions(desc);
  Options::addRandomNumbersOptions(desc);
  Options::addPythia8Options(desc);
  Options::addOutputOptions(desc);
  Options::addEventToTrackConverterOptions(desc);
  auto vm = Options::parse(desc, argc, argv);
  if (vm.empty()) {
    return EXIT_FAILURE;
  }

  auto logLevel = Options::readLogLevel(vm);

  // basic services
  RandomNumbersSvc::Config rndCfg;
  rndCfg.seed  = 123;
  auto rnd     = std::make_shared<RandomNumbersSvc>(rndCfg);
  auto barcode = std::make_shared<BarcodeSvc>(
      BarcodeSvc::Config(), Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // event generation w/ process guns
  EventGenerator::Config evgenCfg = Options::readPythia8Options(vm);
  evgenCfg.output                 = "generated_particles";
  evgenCfg.randomNumbers          = rnd;
  evgenCfg.barcodeSvc             = barcode;

  // Set up constant B-Field
  Acts::ConstantBField bField(Acts::Vector3D(0., 0., 1.) * Acts::units::_T);
  // Set up Eigenstepper
  Acts::EigenStepper<Acts::ConstantBField> stepper(bField);
  // Set up propagator with void navigator
  Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(
      stepper);

  // Set up event to track converter algorithm
  EventToTrackConverterAlgorithm::Config trkConvConfig
      = Options::readEventToTrackConverterConfig(vm);
  trkConvConfig.randomNumberSvc = rnd;
  trkConvConfig.bField          = bField;
  trkConvConfig.inputCollection = evgenCfg.output;

  // Set up track selector
  TrackSelector::Config selectorConfig;
  selectorConfig.input       = trkConvConfig.outputCollection;
  selectorConfig.output      = "selectedTracks";
  selectorConfig.absEtaMax   = 2.5;
  selectorConfig.rhoMax      = 4 * Acts::units::_mm;
  selectorConfig.ptMin       = 400. * Acts::units::_MeV;
  selectorConfig.keepNeutral = false;

  // Set up Billoir Vertex Fitter
  Acts::
      FullBilloirVertexFitter<Acts::ConstantBField,
                              Acts::BoundParameters,
                              Acts::
                                  Propagator<Acts::
                                                 EigenStepper<Acts::
                                                                  ConstantBField>>>::
          Config vertexFitterCfg(bField, propagator);
  auto           billoirFitter = std::
      make_shared<Acts::
                      FullBilloirVertexFitter<Acts::ConstantBField,
                                              Acts::BoundParameters,
                                              Acts::
                                                  Propagator<Acts::
                                                                 EigenStepper<Acts::
                                                                                  ConstantBField>>>>(
          vertexFitterCfg);

  // Add the fit algorithm with Billoir fitter
  FWE::VertexFitAlgorithm::Config vertexFitCfg;
  vertexFitCfg.trackCollection = selectorConfig.output;
  vertexFitCfg.vertexFitter    = billoirFitter;

  Sequencer::Config sequencerCfg = Options::readSequencerConfig(vm);
  Sequencer         sequencer(sequencerCfg);

  sequencer.addReader(std::make_shared<EventGenerator>(evgenCfg, logLevel));

  sequencer.addAlgorithm(std::make_shared<EventToTrackConverterAlgorithm>(
      trkConvConfig, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<TrackSelector>(selectorConfig, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<FWE::VertexFitAlgorithm>(vertexFitCfg, logLevel));

  sequencer.run();

  return 0;
}
