// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <ios>
#include <stdexcept>

#include <Acts/Plugins/Digitization/PlanarModuleCluster.hpp>
#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/EventData/SimIdentifier.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/EventData/SimVertex.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Txt/TxtPlanarClusterWriter.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Txt::TxtPlanarClusterWriter::TxtPlanarClusterWriter(
    const FW::Txt::TxtPlanarClusterWriter::Config& cfg,
    Acts::Logging::Level                           level)
  : Base(cfg.collection, "TxtPlanarClusterWriter", level), m_cfg(cfg)
{
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  }
}

FW::ProcessCode
FW::Txt::TxtPlanarClusterWriter::writeT(
    const AlgorithmContext& context,
    const FW::DetectorData<geo_id_value, Acts::PlanarModuleCluster>& clusters)
{
  // open per-event hits file
  std::string pathHits
      = perEventFilepath(m_cfg.outputDir, "hits.txt", context.eventNumber);
  std::ofstream osHits(pathHits, std::ofstream::out | std::ofstream::trunc);
  if (!osHits) {
    throw std::ios_base::failure("Could not open '" + pathHits + "' to write");
  }

  // open per-event truth file
  std::string pathTruth
      = perEventFilepath(m_cfg.outputDir, "truth.txt", context.eventNumber);
  std::ofstream osTruth(pathTruth, std::ofstream::out | std::ofstream::trunc);
  if (!osTruth) {
    throw std::ios_base::failure("Could not open '" + pathTruth + "' to write");
  }

  auto geoContext = context.geoContext;

  osHits << std::setw(10) << "h";
  osHits << std::setw(10) << "x";
  osHits << std::setw(10) << "y";
  osHits << std::setw(10) << "z";
  osHits << std::setw(5) << "lay";
  osHits << std::setw(5) << "vol";
  osHits << std::setw(6) << "trk";
  osHits << std::setw(9) << "pT";
  osHits << std::setw(4) << "q";
  osHits << std::setw(4) << "v";  // number of hits per track to be added
  osHits << '\n';

  osHits << std::setprecision(3);

  std::map<int, std::map<int, bool>> layerVolumeMap;

  size_t hitId = 0;
  for (auto& volumeData : clusters) {
    for (auto& layerData : volumeData.second) {
      for (auto& moduleData : layerData.second) {
        for (auto& cluster : moduleData.second) {
          // local cluster information
          auto           parameters = cluster.parameters();
          Acts::Vector2D local(parameters[Acts::ParDef::eLOC_0],
                               parameters[Acts::ParDef::eLOC_1]);
          Acts::Vector3D pos(0, 0, 0);
          Acts::Vector3D mom(1, 1, 1);
          // transform local into global position information
          cluster.referenceSurface().localToGlobal(geoContext, local, mom, pos);

          // write hit information
          osHits << std::setw(10) << std::setprecision(3) << std::fixed
                 << hitId++;
          osHits << std::setw(10) << std::setprecision(3) << std::fixed
                 << pos.x();
          osHits << std::setw(10) << std::setprecision(3) << std::fixed
                 << pos.y();
          osHits << std::setw(10) << std::setprecision(3) << std::fixed
                 << pos.z();
          osHits << std::setw(5) << layerData.first;
          osHits << std::setw(5) << volumeData.first;

          /// Hit identifier
          auto hitIdentifier = cluster.sourceLink();
          // write hit-particle truth association
          // each hit can have multiple particles, e.g. in a dense environment
          for (auto& sPartilce : hitIdentifier.truthParticles()) {
            auto particleBarcode = sPartilce->barcode();
            osHits << std::setw(6)
                   << m_cfg.barcodeSvc->primary(particleBarcode);
            osHits << std::setw(9) << std::setprecision(3) << std::fixed
                   << sPartilce->pT();
            osHits << std::setw(4) << int(sPartilce->q());
            osHits << std::setw(4) << m_cfg.barcodeSvc->vertex(particleBarcode);
          }
          osHits << '\n';
        }
      }
    }
  }

  return FW::ProcessCode::SUCCESS;
}
