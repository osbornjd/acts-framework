// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <memory>

#include <Acts/EventData/TrackParameters.hpp>
#include <boost/program_options.hpp>

#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Generators/FlattenEvent.hpp"
#include "ACTFW/Generators/ParticleSelector.hpp"
#include "ACTFW/Generators/Pythia8ProcessGenerator.hpp"
#include "ACTFW/Io/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Io/Root/RootParticleWriter.hpp"
#include "ACTFW/Io/Root/RootVertexAndTracksWriter.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Options/Pythia8Options.hpp"
#include "ACTFW/TruthTracking/ParticleSmearing.hpp"
#include "ACTFW/TruthTracking/ParticleToPerigee.hpp"
#include "ACTFW/TruthTracking/TrackSelector.hpp"
#include "ACTFW/TruthTracking/TruthVerticesToTracks.hpp"
#include "ACTFW/Utilities/Paths.hpp"

using namespace FW;

/// Main vertex finder example executable
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
  auto vm = Options::parse(desc, argc, argv);
  if (vm.empty()) { return EXIT_FAILURE; }

  auto logLevel = Options::readLogLevel(vm);

  // basic services
  auto rndCfg = Options::readRandomNumbersConfig(vm);
  auto rnd    = std::make_shared<RandomNumbers>(rndCfg);

  // Set up event generator producing one single hard collision
  EventGenerator::Config evgenCfg = Options::readPythia8Options(vm, logLevel);
  evgenCfg.output                 = "generatedEvent";
  evgenCfg.randomNumbers          = rnd;

  ParticleSelector::Config ptcSelectorCfg;
  ptcSelectorCfg.input       = evgenCfg.output;
  ptcSelectorCfg.output      = "selectedParticles";
  ptcSelectorCfg.absEtaMax   = 2.5;
  ptcSelectorCfg.rhoMax      = 4_mm;
  ptcSelectorCfg.ptMin       = 400._MeV;
  ptcSelectorCfg.keepNeutral = false;

  FlattenEvent::Config flattenEventCfg;
  flattenEventCfg.inputEvent      = ptcSelectorCfg.output;
  flattenEventCfg.outputParticles = "flattenedParticles";

  // Create smeared particles states
  ParticleSmearing::Config particleSmearingCfg;
  particleSmearingCfg.inputParticles        = flattenEventCfg.outputParticles;
  particleSmearingCfg.outputTrackParameters = "smearedparameters";
  particleSmearingCfg.randomNumbers         = rnd;
  // Gaussian sigmas to smear particle parameters
  particleSmearingCfg.sigmaD0    = 20_um;
  particleSmearingCfg.sigmaD0PtA = 30_um;
  particleSmearingCfg.sigmaD0PtB = 0.3 / 1_GeV;
  particleSmearingCfg.sigmaZ0    = 20_um;
  particleSmearingCfg.sigmaZ0PtA = 30_um;
  particleSmearingCfg.sigmaZ0PtB = 0.3 / 1_GeV;
  particleSmearingCfg.sigmaPhi   = 1_degree;
  particleSmearingCfg.sigmaTheta = 1_degree;
  particleSmearingCfg.sigmaPRel  = 0.01;
  particleSmearingCfg.sigmaT0    = 1_ns;

  // Set magnetic field
  Acts::Vector3D bField(0., 0., 2.08_T);

  ParticleToPerigee::Config particleToPerigeeCfg;
  particleToPerigeeCfg.inputParameters
      = particleSmearingCfg.outputTrackParameters;
  particleToPerigeeCfg.outputPerigee = "perigeeParameters";
  particleToPerigeeCfg.bField        = bField;

  // Set up TruthVerticesToTracks converter algorithm
  TruthVerticesToTracksAlgorithm::Config trkConvCfg;

  trkConvCfg.inputParticles = flattenEventCfg.outputParticles;
  trkConvCfg.inputPerigees  = particleToPerigeeCfg.outputPerigee;
  trkConvCfg.output         = "allTracks";

  RootVertexAndTracksWriter::Config writerCfg;
  writerCfg.collection = trkConvCfg.output;

  int         nPileup        = vm["evg-pileup"].template as<int>();
  int         nEvents        = vm["events"].as<size_t>();
  std::string pileupString   = std::to_string(nPileup);
  std::string nEventsString  = std::to_string(nEvents);
  std::string outputDir      = vm["output-dir"].as<std::string>();
  std::string outputFilePath = outputDir + "VertexAndTracksCollection_n"
      + nEventsString + "_p" + pileupString + ".root";
  writerCfg.filePath = outputFilePath;

  Sequencer::Config sequencerCfg = Options::readSequencerConfig(vm);
  Sequencer         sequencer(sequencerCfg);

  sequencer.addReader(std::make_shared<EventGenerator>(evgenCfg, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<ParticleSelector>(ptcSelectorCfg, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<FlattenEvent>(flattenEventCfg, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<ParticleSmearing>(particleSmearingCfg, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<ParticleToPerigee>(particleToPerigeeCfg, logLevel));

  sequencer.addAlgorithm(
      std::make_shared<TruthVerticesToTracksAlgorithm>(trkConvCfg, logLevel));

  sequencer.addWriter(
      std::make_shared<RootVertexAndTracksWriter>(writerCfg, logLevel));

  return sequencer.run();
}
