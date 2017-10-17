// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <fstream>
#include <ios>
#include <stdexcept>
#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Csv::CsvParticleWriter::CsvParticleWriter(
    const FW::Csv::CsvParticleWriter::Config& cfg,
    Acts::Logging::Level                      level)
  : Base(cfg.collection, "CsvParticleWriter", level), m_cfg(cfg)
{
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  } else if (!m_cfg.barcodeSvc) {
    throw std::invalid_argument("Missing barcode service");
  }
}

FW::ProcessCode
FW::Csv::CsvParticleWriter::writeT(
    const FW::AlgorithmContext&                  ctx,
    const std::vector<Acts::ProcessVertex>& vertices)
{
  std::string path
      = perEventFilepath(m_cfg.outputDir, "particles.csv", ctx.eventNumber);
  std::ofstream os(path, std::ofstream::out | std::ofstream::trunc);
  if (!os) {
    throw std::ios_base::failure("Could not open '" + path + "' to write");
  }

  // write csv header
  os << "particle_id,";
  os << "vx,vy,vz,";
  os << "px,py,pz,";
  os << "q\n";

  // write one line per particle
  os << std::setprecision(m_cfg.outputPrecision);
  for (auto& vertex: vertices){
    auto& vtx = vertex.position();
    for (auto& particle : vertex.outgoingParticles()) {
      os << particle.barcode() << ",";
      os << vtx.x() << ",";
      os << vtx.y() << ",";
      os << vtx.z() << ",";
      os << particle.momentum().x() << ",";
      os << particle.momentum().y() << ",";
      os << particle.momentum().z() << ",";
      os << particle.charge() << '\n';
    }
  }
  return ProcessCode::SUCCESS;
}
