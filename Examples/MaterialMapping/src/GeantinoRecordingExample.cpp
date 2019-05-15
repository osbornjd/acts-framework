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
#include "ACTFW/MaterialMapping/GeantinoRecording.hpp"
#include "ACTFW/Plugins/DD4hepG4/DD4hepToG4Svc.hpp"
#include "ACTFW/Plugins/Root/RootMaterialTrackWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "ACTFW/Writers/IWriterT.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/GeometryContext.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{
  // Declare the supported program options.
  po::options_description desc("Allowed options");
  // Add the standard/common options
  FW::Options::addCommonOptions<po::options_description>(desc);
  // Add the output options
  FW::Options::addOutputOptions<po::options_description>(desc);
  // Add the detector options
  FW::Options::addDD4hepOptions<po::options_description>(desc);
  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
  // Now read the standard options
  auto nEvents = FW::Options::readNumberOfEvents<po::variables_map>(vm);

  size_t nTracks     = 100;
  int    randomSeed1 = 536235167;
  int    randomSeed2 = 729237523;

  Acts::GeometryContext geoContext;

  // DETECTOR:
  // --------------------------------------------------------------------------------
  // DD4Hep detector definition
  // read the detector config & dd4hep detector
  auto dd4HepDetectorConfig
      = FW::Options::readDD4hepConfig<po::variables_map>(vm);
  auto geometrySvc = std::make_shared<FW::DD4hep::DD4hepGeometryService>(
      dd4HepDetectorConfig);
  std::shared_ptr<const Acts::TrackingGeometry> tGeometry
      = geometrySvc->trackingGeometry(geoContext);

  // DD4Hep to Geant4 conversion
  //
  FW::DD4hepG4::DD4hepToG4Svc::Config dgConfig("DD4hepToG4",
                                               Acts::Logging::INFO);
  dgConfig.dd4hepService = geometrySvc;
  auto dd4hepToG4Svc = std::make_shared<FW::DD4hepG4::DD4hepToG4Svc>(dgConfig);

  // --------------------------------------------------------------------------------
  // Geant4 JOB:
  // --------------------------------------------------------------------------------
  // set up the writer for

  // ---------------------------------------------------------------------------------

  // set up the algorithm writing out the material map
  FW::GeantinoRecording::Config g4rConfig;
  g4rConfig.geant4Service  = dd4hepToG4Svc;
  g4rConfig.tracksPerEvent = nTracks;
  g4rConfig.seed1          = randomSeed1;
  g4rConfig.seed2          = randomSeed2;
  // create the geant4 algorithm
  auto g4rAlgorithm
      = std::make_shared<FW::GeantinoRecording>(g4rConfig, Acts::Logging::INFO);

  // Geant4 job - these can be many Geant4 jobs, indeed
  //
  // create the config object for the sequencer
  FW::Sequencer::Config g4SeqConfig;
  // now create the sequencer
  FW::Sequencer g4Sequencer(g4SeqConfig);

  // Output directory
  std::string outputDir     = vm["output-dir"].template as<std::string>();
  std::stirng matCollection = g4rConfig.geantMaterialCollection;

  if (vm["output-root"].template as<bool>()) {
    // Write the propagation steps as ROOT TTree
    FW::Root::RootMaterialTrackWriter::Config matTrackWriterRootConfig;
    matTrackWriterRootConfig.collection = matCollection;
    matTrackWriterRootConfig.filePath
        = FW::joinPaths(outputDir, matCollection + ".root");
    auto matTrackWriterRoot
        = std::make_shared<FW::Root::RootMaterialTrackWriter>(
            matTrackWriterRootConfig);
    if (g4Sequencer.addWriters({matTrackWriterRoot})
        != FW::ProcessCode::SUCCESS)
      return -1;
  }

  // Append the algorithm and run
  g4Sequencer.appendEventAlgorithms({g4rAlgorithm});
  g4Sequencer.run(nEvents);
}
