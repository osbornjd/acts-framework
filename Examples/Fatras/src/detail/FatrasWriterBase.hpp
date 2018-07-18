// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <boost/program_options.hpp>
#include "ACTFW/Plugins/Root/RootFatrasHitWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Utilities/Paths.hpp"

namespace po = boost::program_options;
//#include "ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp"

FW::ProcessCode
setupRootWriters(po::variables_map&              vm,
                 FW::Sequencer&                  sequencer,
                 std::shared_ptr<FW::BarcodeSvc> barcodeSvc)
{
  const std::string particles = "EvgenParticles";
  const std::string hits      = "FatrasHits";
  const std::string clusters  = "FatrasClusters";
  const std::string points    = "FatrasSpacePoints";
  const std::string outputDir = "";

  // particles as ROOT TTree
  FW::Root::RootParticleWriter::Config pWriterRootConfig;
  pWriterRootConfig.collection = particles;
  pWriterRootConfig.filePath   = FW::joinPaths(outputDir, "particles.root");
  pWriterRootConfig.barcodeSvc = barcodeSvc;
  auto particleWriterRoot
      = std::make_shared<FW::Root::RootParticleWriter>(pWriterRootConfig);
  if (sequencer.addWriters({particleWriterRoot}) != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;

  // simulated hits as ROOT TTree
  FW::Root::RootFatrasHitWriter::Config fhitWriterRootConfig;
  fhitWriterRootConfig.collection = hits;
  fhitWriterRootConfig.filePath
      = FW::joinPaths(outputDir, "simulated-hits.root");
  auto fhitWriterRoot
      = std::make_shared<FW::Root::RootFatrasHitWriter>(fhitWriterRootConfig);
  if (sequencer.addWriters({fhitWriterRoot}) != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;

  // hits as ROOT TTree

  // clusters as root
  // FW::Root::RootPlanarClusterWriter::Config clusterWriterRootConfig;
  // clusterWriterRootConfig.collection = clusters;
  // clusterWriterRootConfig.filePath = FW::joinPaths(outputDir,
  // "clusters.root");
  // auto clusteWriterRoot
  //   = production ? nullptr :
  //     std::make_shared<FW::Root::RootPlanarClusterWriter>(
  //     clusterWriterRootConfig);

  return FW::ProcessCode::SUCCESS;
}
