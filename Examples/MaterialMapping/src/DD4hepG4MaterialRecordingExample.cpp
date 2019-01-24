// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include "ACTFW/Common/CommonOptions.hpp"
#include "ACTFW/Common/OutputOptions.hpp"
#include "ACTFW/DD4hepDetector/DD4hepDetectorOptions.hpp"
#include "ACTFW/DD4hepDetector/DD4hepGeometryService.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/MaterialMapping/Geant4MaterialRecording.hpp"
#include "ACTFW/MaterialMapping/MaterialMappingOptions.hpp"
#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Utilities/Paths.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the detector options
  FW::Options::addDD4hepOptions<po::options_description>(desc);
  // Add the material mapping options
  FW::Options::addMaterialMappingOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);
  // Prepare the variables map and go on
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  // The Log level
  auto nEvents  = FW::Options::readNumberOfEvents<po::variables_map>(vm);
  auto logLevel = FW::Options::readLogLevel<po::variables_map>(vm);

  // DETECTOR:
  // --------------------------------------------------------------------------------
  // DD4Hep detector definition
  // read the detector config & dd4hep detector
  auto dd4HepDetectorConfig
      = FW::Options::readDD4hepConfig<po::variables_map>(vm);
  auto geometrySvc = std::make_shared<FW::DD4hep::DD4hepGeometryService>(
      dd4HepDetectorConfig);

  // DD4Hep to Geant4 conversion
  //
  FW::DD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4", logLevel);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc = std::make_shared<FW::DD4hepG4::DD4hepToG4Svc>(dgConfig);

  // --------------------------------------------------------------------------------
  // Geant4 JOB:
  // --------------------------------------------------------------------------------

  auto g4rAlgConfig
      = FW::Options::readG4MaterialRecordingConfig<po::variables_map>(vm);
  g4rAlgConfig.geant4Service = dd4hepToG4Svc;

  // create the geant4 algorithm
  auto g4rAlgorithm
      = std::make_shared<FW::Geant4MaterialRecording>(g4rAlgConfig, logLevel);

  // Geant4 job - these can be many Geant4 jobs, indeed
  //
  // create the config object for the sequencer
  FW::Sequencer::Config seqConfig;
  FW::Sequencer         sequencer(seqConfig);

  // ---------------------------------------------------------------------------------
  // Output directory
  std::string outputDir     = vm["output-dir"].template as<std::string>();
  auto        matCollection = vm["mm-g4-material-collection"].as<std::string>();

  if (vm["output-root"].template as<bool>()) {
    // Write the propagation steps as ROOT TTree
    FW::Root::RootMaterialTrackWriter::Config matTrackWriterRootConfig;
    matTrackWriterRootConfig.recalculateTotals = true;
    matTrackWriterRootConfig.collection        = matCollection;
    matTrackWriterRootConfig.filePath
        = FW::joinPaths(outputDir, matCollection + ".root");
    auto matTrackWriterRoot
        = std::make_shared<FW::Root::RootMaterialTrackWriter>(
            matTrackWriterRootConfig);
    if (sequencer.addWriters({matTrackWriterRoot}) != FW::ProcessCode::SUCCESS)
      return -1;
  }

  // the writer is a service as it needs initialize, finalize
  sequencer.appendEventAlgorithms({g4rAlgorithm});
  // Initiate the run
  sequencer.run(nEvents);
  // Return success code
  return 0;
}
