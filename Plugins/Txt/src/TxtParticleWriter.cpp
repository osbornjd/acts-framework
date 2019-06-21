// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Txt/TxtParticleWriter.hpp"
#include <fstream>
#include <ios>
#include <stdexcept>
#include "ACTFW/Barcode/Barcode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Txt::TxtParticleWriter::TxtParticleWriter(
    const FW::Txt::TxtParticleWriter::Config& cfg,
    Acts::Logging::Level                      level)
  : ParticleWriter(cfg.collection, "TxtParticleWriter", level), m_cfg(cfg)
{
  if (m_cfg.collection.empty()) {
    throw std::invalid_argument("Missing input collection");
  }
}

FW::ProcessCode
FW::Txt::TxtParticleWriter::writeT(
    const FW::AlgorithmContext&           ctx,
    const std::vector<Data::SimVertex<>>& vertices)
{
  std::string pathOsTrack = perEventFilepath(
      m_cfg.outputDir, m_cfg.outputTrkFileName, ctx.eventNumber);
  std::ofstream osTrack(pathOsTrack, std::ofstream::out | std::ofstream::trunc);
  if (!osTrack) {
    throw std::ios_base::failure("Could not open '" + pathOsTrack
                                 + "' to write");
  }

  std::string pathOsVertex = perEventFilepath(
      m_cfg.outputDir, m_cfg.outputVtxFileName, ctx.eventNumber);
  std::ofstream osVertex(pathOsVertex,
                         std::ofstream::out | std::ofstream::trunc);
  if (!osVertex) {
    throw std::ios_base::failure("Could not open '" + pathOsVertex
                                 + "' to write");
  }

  // write csv header
  osTrack << std::setw(4) << "trk";
  osTrack << std::setw(4) << "v";
  osTrack << std::setw(9) << "phi";
  osTrack << std::setw(9) << "theta";
  osTrack << std::setw(9) << "pT";
  osTrack << std::setw(4) << "q";
  osTrack << '\n';

  // write csv header
  osVertex << std::setw(4) << "v";
  osVertex << std::setw(9) << "x";
  osVertex << std::setw(9) << "y";
  osVertex << std::setw(9) << "z";
  osVertex << '\n';

  // write one line per particle
  osTrack << std::setprecision(m_cfg.outputPrecision);
  for (auto& vertex : vertices) {
    bool writeVertex = true;
    for (auto& particle : vertex.outgoing()) {
      // particle barcode
      auto particleBarcode = particle.barcode();
      auto vertexNr        = m_cfg.barcodeSvc->vertex(particleBarcode);
      if (writeVertex) {
        if (abs(vertex.position.z()) < 500.) {
          osVertex << std::setw(4) << vertexNr;
          osVertex << std::setw(9) << std::setprecision(3) << std::fixed
                   << vertex.position.x();
          osVertex << std::setw(9) << std::setprecision(3) << std::fixed
                   << vertex.position.y();
          osVertex << std::setw(9) << std::setprecision(3) << std::fixed
                   << vertex.position.z();
          osVertex << '\n';
        }
        writeVertex = false;
      }
      auto particleNr = m_cfg.barcodeSvc->primary(particleBarcode);
      osTrack << std::setw(4) << particleNr;
      osTrack << std::setw(4) << vertexNr;
      osTrack << std::setw(9) << std::setprecision(3) << std::fixed
              << Acts::VectorHelpers::phi(particle.momentum());
      osTrack << std::setw(9) << std::setprecision(3) << std::fixed
              << Acts::VectorHelpers::theta(particle.momentum());
      osTrack << std::setw(9) << std::setprecision(3) << std::fixed
              << particle.pT();
      osTrack << std::setw(4) << int(particle.q());
      osTrack << '\n';
    }
  }
  return ProcessCode::SUCCESS;
}
