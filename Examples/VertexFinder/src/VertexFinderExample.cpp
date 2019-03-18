// This file is part of the Acts project.
//
// Copyright (C) 2016-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Pythia8/Generator.hpp"
#include "ACTFW/Plugins/Pythia8/GeneratorOptions.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Random/RandomNumbersOptions.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/ReadEvgen/EvgenReader.hpp"
#include "ACTFW/ReadEvgen/ReadEvgenOptions.hpp"

#include "Acts/EventData/TrackParameters.hpp"
#include "Acts/MagneticField/ConstantBField.hpp"
#include "Acts/Propagator/EigenStepper.hpp"
#include "Acts/Propagator/Propagator.hpp"
#include "Acts/Vertexing/VertexFinderTools/IterativeVertexFinder.hpp"
#include "Acts/Vertexing/VertexFitterTools/FullBilloirVertexFitter.hpp"

#include "VertexFinderAlgorithm.hpp"

namespace po = boost::program_options;

/// Main read evgen executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the random number options
  FW::Options::addRandomNumbersOptions<po::options_description>(desc);
  // Add the pythia 8 options
  FW::Options::addPythia8Options<po::options_description>(desc);
  // Add the evgen options
  FW::Options::addEvgenReaderOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);
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
  // Read the common options
  auto nEvents  = FW::Options::readNumberOfEvents<po::variables_map>(vm);
  auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

  // Create the random number engine
  auto randomNumberSvcCfg
      = FW::Options::readRandomNumbersConfig<po::variables_map>(vm);
  auto randomNumberSvc
      = std::make_shared<FW::RandomNumbersSvc>(randomNumberSvcCfg);
  // Now read the pythia8 configs
  auto pythia8Configs = FW::Options::readPythia8Config<po::variables_map>(vm);
  pythia8Configs.first.randomNumberSvc  = randomNumberSvc;
  pythia8Configs.second.randomNumberSvc = randomNumberSvc;
  // The hard scatter generator
  auto hsPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
      pythia8Configs.first,
      Acts::getDefaultLogger("HardScatterPythia8Generator", logLevel));
  // The pileup generator
  auto puPythiaGenerator = std::make_shared<FW::GPythia8::Generator>(
      pythia8Configs.second,
      Acts::getDefaultLogger("PileUpPythia8Generator", logLevel));
  // Create the barcode service
  FW::BarcodeSvc::Config barcodeSvcCfg;
  auto                   barcodeSvc = std::make_shared<FW::BarcodeSvc>(
      barcodeSvcCfg, Acts::getDefaultLogger("BarcodeSvc", logLevel));

  // Set up constant B-Field
  Acts::ConstantBField bField(Acts::Vector3D(0., 0., 1.) * Acts::units::_T);

  // Set up Eigenstepper
  Acts::EigenStepper<Acts::ConstantBField> stepper(bField);

  // Set up propagator with void navigator
  Acts::Propagator<Acts::EigenStepper<Acts::ConstantBField>> propagator(
      stepper);

  typedef Acts::
      FullBilloirVertexFitter<Acts::ConstantBField,
                              Acts::BoundParameters,
                              Acts::
                                  Propagator<Acts::
                                                 EigenStepper<Acts::
                                                                  ConstantBField>>>
          BilloirFitter;

  // Set up Billoir Vertex Fitter
  BilloirFitter::Config vertexFitterCfg(bField);

  std::unique_ptr<BilloirFitter> bFitterPtr

      = std::make_unique<BilloirFitter>(vertexFitterCfg);

  // Vertex Finder
  typedef Acts::
      IterativeVertexFinder<Acts::ConstantBField,
                            Acts::BoundParameters,
                            Acts::
                                Propagator<Acts::
                                               EigenStepper<Acts::
                                                                ConstantBField>>>
                       VertexFinder;
  VertexFinder::Config finderCfg(bField, std::move(bFitterPtr), propagator);
  finderCfg.reassignTracksAfterFirstFit = true;

  auto vertexFinder = std::make_shared<VertexFinder>(finderCfg);

  // Now read the evgen config & set the missing parts
  auto readEvgenCfg                   = FW::Options::readEvgenConfig(vm);
  readEvgenCfg.hardscatterEventReader = hsPythiaGenerator;
  readEvgenCfg.pileupEventReader      = puPythiaGenerator;
  readEvgenCfg.randomNumberSvc        = randomNumberSvc;
  readEvgenCfg.barcodeSvc             = barcodeSvc;
  readEvgenCfg.nEvents                = nEvents;

  // Create the read Algorithm
  auto readEvgen = std::make_shared<FW::EvgenReader>(
      readEvgenCfg, Acts::getDefaultLogger("EvgenReader", logLevel));

  // Add the fit algorithm with Billoir fitter
  FWE::VertexFinderAlgorithm::Config vertexFinderCfg;
  vertexFinderCfg.collection      = readEvgenCfg.evgenCollection;
  vertexFinderCfg.randomNumberSvc = randomNumberSvc;
  vertexFinderCfg.vertexFinder    = vertexFinder;
  vertexFinderCfg.bField          = bField;

  std::shared_ptr<FW::IAlgorithm> vertexFitAlgo
      = std::make_shared<FWE::VertexFinderAlgorithm>(vertexFinderCfg, logLevel);

  // Create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  // Now create the sequencer
  FW::Sequencer sequencer(seqConfig);
  sequencer.addServices({randomNumberSvc});
  sequencer.addReaders({readEvgen});
  sequencer.appendEventAlgorithms({vertexFitAlgo});

  sequencer.run(nEvents);

  return 0;
}
