// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <memory>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/Framework/Sequencer.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/GenericDetector/GenericDetector.hpp"
#include "ACTFW/Geometry/CommonGeometry.hpp"
#include "ACTFW/Options/CommonOptions.hpp"
#include "ACTFW/Plugins/Csv/CsvOptionsReader.hpp"
#include "ACTFW/Plugins/Csv/CsvParticleReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterReader.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Utilities/Options.hpp"

int
main(int argc, char* argv[])
{
  GenericDetector detector;

  // setup and parse options
  auto desc = FW::Options::makeDefaultOptions();
  FW::Options::addSequencerOptions(desc);
  FW::Options::addGeometryOptions(desc);
  FW::Options::addMaterialOptions(desc);
  FW::Options::addInputOptions(desc);
  detector.addOptions(desc);

  auto vm = FW::Options::parse(desc, argc, argv);
  if (vm.empty()) { return EXIT_FAILURE; }

  FW::Sequencer sequencer(FW::Options::readSequencerConfig(vm));

  // Read some standard options
  auto logLevel = FW::Options::readLogLevel(vm);
  auto inputDir = vm["input-dir"].as<std::string>();

  // Setup detector geometry
  auto geometry         = FW::Geometry::build(vm, detector);
  auto trackingGeometry = geometry.first;
  // Add context decorators
  for (auto cdr : geometry.second) { sequencer.addContextDecorator(cdr); }

  // Read particles from CSV files
  auto particleReaderCfg = FW::Options::readCsvParticleReaderConfig(vm);
  particleReaderCfg.outputParticles = "particles";
  sequencer.addReader(std::make_shared<FW::Csv::CsvParticleReader>(
      particleReaderCfg, logLevel));

  // Read clusters from CSV files
  auto clusterReaderCfg = FW::Options::readCsvPlanarClusterReaderConfig(vm);
  clusterReaderCfg.trackingGeometry = trackingGeometry;
  clusterReaderCfg.outputClusters   = "clusters";
  sequencer.addReader(std::make_shared<FW::Csv::CsvPlanarClusterReader>(
      clusterReaderCfg, logLevel));

  return sequencer.run();
}
