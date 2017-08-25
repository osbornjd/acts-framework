// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "FatrasCommon.hpp"

#include <ACTS/MagneticField/ConstantBField.hpp>
#include <ACTS/Utilities/Units.hpp>
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Plugins/Csv/CsvPlanarClusterWriter.hpp"
#include "ACTFW/Plugins/Json/JsonSpacePointWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjSpacePointWriter.hpp"
#include "ACTFW/Plugins/Root/RootExCellWriter.hpp"
#include "ACTFW/Plugins/Root/RootParticleWriter.hpp"
#include "ACTFW/Plugins/Root/RootPlanarClusterWriter.hpp"
#include "Fatras/EnergyLossSampler.hpp"
#include "Fatras/MaterialInteractionEngine.hpp"
#include "Fatras/MultipleScatteringSamplerHighland.hpp"

FW::ProcessCode
setupWriters(FW::Sequencer&                  sequencer,
             std::shared_ptr<FW::BarcodeSvc> barcode,
             std::string                     outputDir,
             Acts::Logging::Level            defaultLevel)
{
  const std::string particles = "FatrasParticles";
  const std::string clusters  = "FatrasClusters";
  const std::string points    = "FatrasSpacePoints";

  // particles as csv
  FW::Csv::CsvParticleWriter::Config pWriterCsvConfig;
  pWriterCsvConfig.collection = particles;
  pWriterCsvConfig.outputDir  = outputDir;
  pWriterCsvConfig.barcodeSvc = barcode;
  auto pWriterCsv
      = std::make_shared<FW::Csv::CsvParticleWriter>(pWriterCsvConfig);

  // particles as ROOT TTree
  FW::Root::RootParticleWriter::Config pWriterRootConfig;
  pWriterRootConfig.collection = particles;
  pWriterRootConfig.filePath   = FW::joinPaths(outputDir, "particles.root");
  pWriterRootConfig.barcodeSvc = barcode;
  auto pWriterRoot
      = std::make_shared<FW::Root::RootParticleWriter>(pWriterRootConfig);

  // clusters as csv
  FW::Csv::CsvPlanarClusterWriter::Config clusterWriterCsvConfig;
  clusterWriterCsvConfig.collection = clusters;
  clusterWriterCsvConfig.outputDir  = outputDir;
  auto clusterWriterCsv = std::make_shared<FW::Csv::CsvPlanarClusterWriter>(
      clusterWriterCsvConfig);

  // clusters as root
  FW::Root::RootPlanarClusterWriter::Config clusterWriterRootConfig;
  clusterWriterRootConfig.collection = clusters;
  clusterWriterRootConfig.filePath = FW::joinPaths(outputDir, "clusters.root");
  auto clusteWriterRoot = std::make_shared<FW::Root::RootPlanarClusterWriter>(
      clusterWriterRootConfig);

  // space points as json
  FW::Json::JsonSpacePointWriter<Acts::Vector3D>::Config spWriterJsonConfig;
  spWriterJsonConfig.collection = points;
  spWriterJsonConfig.outputDir  = outputDir;
  auto spWriterJson
      = std::make_shared<FW::Json::JsonSpacePointWriter<Acts::Vector3D>>(
          spWriterJsonConfig);

  // space points as obj
  FW::Obj::ObjSpacePointWriter<Acts::Vector3D>::Config spWriterObjConfig;
  spWriterObjConfig.collection = points;
  spWriterObjConfig.outputDir  = outputDir;
  auto spWriterObj
      = std::make_shared<FW::Obj::ObjSpacePointWriter<Acts::Vector3D>>(
          spWriterObjConfig);

  // Write ROOT TTree
  // ecc for charged particles
  FW::Root::RootExCellWriter<Acts::TrackParameters>::Config reccWriterConfig;
  reccWriterConfig.filePath       = "excells_charged.root";
  reccWriterConfig.treeName       = "extrapolation_charged";
  reccWriterConfig.collection     = "excells_charged";
  reccWriterConfig.writeBoundary  = false;
  reccWriterConfig.writeMaterial  = true;
  reccWriterConfig.writeSensitive = true;
  reccWriterConfig.writePassive   = true;
  auto rootEccWriter
      = std::make_shared<FW::Root::RootExCellWriter<Acts::TrackParameters>>(
          reccWriterConfig);

  // ecc for neutral particles
  FW::Root::RootExCellWriter<Acts::NeutralParameters>::Config recnWriterConfig;
  recnWriterConfig.filePath       = "excells_neutral.root";
  recnWriterConfig.treeName       = "extrapolation_neutral";
  recnWriterConfig.collection     = "excells_neutral";
  recnWriterConfig.writeBoundary  = false;
  recnWriterConfig.writeMaterial  = true;
  recnWriterConfig.writeSensitive = true;
  recnWriterConfig.writePassive   = true;
  auto rootEcnWriter
      = std::make_shared<FW::Root::RootExCellWriter<Acts::NeutralParameters>>(
          recnWriterConfig);

  // add to sequencer
  if (sequencer.addWriters({pWriterCsv,
                            pWriterRoot,
                            clusterWriterCsv,
                            clusteWriterRoot,
                            spWriterJson,
                            spWriterObj,
                            rootEccWriter,
                            rootEcnWriter})
      != FW::ProcessCode::SUCCESS)
    return FW::ProcessCode::ABORT;
  return FW::ProcessCode::SUCCESS;
}
