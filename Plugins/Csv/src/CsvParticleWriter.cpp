// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvParticleWriter.hpp"

#include <map>
#include <stdexcept>

#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "TrackMlData.hpp"

FW::Csv::CsvParticleWriter::CsvParticleWriter(
    const FW::Csv::CsvParticleWriter::Config& cfg,
    Acts::Logging::Level                      level)
  : Base(cfg.input, "CsvParticleWriter", level), m_cfg(cfg)
{
  if (m_cfg.input.empty()) {
    throw std::invalid_argument("Missing input collection");
  }
  if (m_cfg.outputFilename.empty()) {
    throw std::invalid_argument("Missing ouput file suffix");
  }
}

FW::ProcessCode
FW::Csv::CsvParticleWriter::writeT(
    const FW::AlgorithmContext&           context,
    const std::vector<Data::SimVertex<>>& vertices)
{
  // use pointer instead of reference since it is optional
  const std::map<barcode_type, size_t>* hitsPerParticle = nullptr;
  if (not m_cfg.inputHitsPerParticle.empty()) {
    hitsPerParticle = &context.eventStore.get<std::map<barcode_type, size_t>>(
        m_cfg.inputHitsPerParticle);
  }

  std::string pathParticles = perEventFilepath(
      m_cfg.outputDir, m_cfg.outputFilename, context.eventNumber);
  dfe::CsvNamedTupleWriter<ParticleData> writer(pathParticles,
                                                m_cfg.outputPrecision);

  ParticleData data;
  data.nhits = -1;  // default for every entry if information unvailable
  for (auto& vertex : vertices) {
    for (auto& particle : vertex.outgoing()) {
      data.particle_id   = particle.barcode();
      data.particle_type = particle.pdg();
      // TODO units
      data.x  = particle.position().x();
      data.y  = particle.position().y();
      data.z  = particle.position().z();
      data.t  = 0;  // TODO
      data.px = particle.momentum().x();
      data.py = particle.momentum().y();
      data.pz = particle.momentum().z();
      data.q  = particle.q();
      // add the hits per particle
      if (hitsPerParticle) {
        auto hppEntry = hitsPerParticle->find(particle.barcode());
        if (hppEntry != hitsPerParticle->end()) {
          data.nhits = hppEntry->second;
        } else {
          data.nhits = -1;
        }
      }
      writer.append(data);
    }
  }

  return ProcessCode::SUCCESS;
}
