// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Csv/CsvParticleReader.hpp"

#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>

#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/Barcode.hpp"
#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvReader.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "TrackMlData.hpp"

FW::Csv::CsvParticleReader::CsvParticleReader(
    const FW::Csv::CsvParticleReader::Config& cfg,
    Acts::Logging::Level                      level)
  : m_cfg(cfg)
  , m_numEvents(
        determineEventFilesRange(cfg.inputDir, cfg.inputFilename).second)
  , m_logger(Acts::getDefaultLogger("CsvParticleReader", level))
{
  if (m_cfg.output.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
  if (m_cfg.inputFilename.empty()) {
    throw std::invalid_argument("Missing input file suffix");
  }
}

std::string
FW::Csv::CsvParticleReader::CsvParticleReader::name() const
{
  return "CsvParticleReader";
}

size_t
FW::Csv::CsvParticleReader::CsvParticleReader::numEvents() const
{
  return m_numEvents;
}

FW::ProcessCode
FW::Csv::CsvParticleReader::read(const FW::AlgorithmContext& ctx)
{
  Data::SimParticles particles;

  dfe::CsvNamedTupleReader<ParticleData> reader(
      perEventFilepath(m_cfg.inputDir, m_cfg.inputFilename, ctx.eventNumber));
  ParticleData data;

  while (reader.read(data)) {
    Acts::Vector3D vertex   = Acts::Vector3D(data.x, data.y, data.z);
    Acts::Vector3D momentum = Acts::Vector3D(data.px, data.py, data.pz);
    //@TODO: get mass and pdg from config?
    double mass = 0.;
    // the file is usually ordered by particle id already
    particles.emplace_hint(particles.end(),
                           vertex,
                           momentum,
                           mass,
                           data.q,
                           data.particle_type,
                           data.particle_id,  // this is the pdg id
                           data.t);
  }

  // write the truth particles to the EventStore
  ctx.eventStore.add(m_cfg.output, std::move(particles));

  return ProcessCode::SUCCESS;
}
