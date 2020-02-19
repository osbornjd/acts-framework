// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Io/Csv/CsvParticleReader.hpp"

#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>

#include <Acts/Utilities/Units.hpp>
#include <dfe/dfe_io_dsv.hpp>

#include "ACTFW/EventData/SimParticle.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Utilities/Paths.hpp"
#include "TrackMlData.hpp"

FW::CsvParticleReader::CsvParticleReader(
    const FW::CsvParticleReader::Config& cfg,
    Acts::Logging::Level                 lvl)
  : m_cfg(cfg)
  , m_eventsRange(
        determineEventFilesRange(cfg.inputDir, cfg.inputStem + ".csv"))
  , m_logger(Acts::getDefaultLogger("CsvParticleReader", lvl))
{
  if (m_cfg.outputParticles.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
  if (m_cfg.inputStem.empty()) {
    throw std::invalid_argument("Missing input filename stem");
  }
}

std::string
FW::CsvParticleReader::CsvParticleReader::name() const
{
  return "CsvParticleReader";
}

std::pair<size_t, size_t>
FW::CsvParticleReader::availableEvents() const
{
  return m_eventsRange;
}

FW::ProcessCode
FW::CsvParticleReader::read(const FW::AlgorithmContext& ctx)
{
  SimParticles::sequence_type unordered;

  auto path = perEventFilepath(
      m_cfg.inputDir, m_cfg.inputStem + ".csv", ctx.eventNumber);
  // vt and m are an optional columns
  dfe::NamedTupleCsvReader<ParticleData> reader(path, {"vt", "m"});
  ParticleData                           data;

  while (reader.read(data)) {
    Acts::Vector3D particlePos(data.vx * Acts::UnitConstants::mm,
                               data.vy * Acts::UnitConstants::mm,
                               data.vz * Acts::UnitConstants::mm);
    Acts::Vector3D particleMom(data.px * Acts::UnitConstants::GeV,
                               data.py * Acts::UnitConstants::GeV,
                               data.pz * Acts::UnitConstants::GeV);
    unordered.emplace_back(particlePos,
                           particleMom,
                           data.m * Acts::UnitConstants::GeV,
                           data.q * Acts::UnitConstants::e,
                           data.particle_type,  // this is the pdg id
                           data.particle_id,
                           data.vt * Acts::UnitConstants::ns);
  }

  // write ordered particles container to the EventStore
  SimParticles particles;
  particles.adopt_sequence(std::move(unordered));
  ctx.eventStore.add(m_cfg.outputParticles, std::move(particles));

  return ProcessCode::SUCCESS;
}
