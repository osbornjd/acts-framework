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
#include "ACTFW/Barcode/Barcode.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"
#include "ACTFW/Plugins/Csv/CsvReader.hpp"
#include "ACTFW/Utilities/Paths.hpp"

FW::Csv::CsvParticleReader::CsvParticleReader(
    const FW::Csv::CsvParticleReader::Config& cfg,
    Acts::Logging::Level                      level)
  : m_cfg(cfg), m_logger(Acts::getDefaultLogger("CsvParticleReader", level))
{
  if (m_cfg.outputParticleCollection.empty()) {
    throw std::invalid_argument("Missing output collection");
  }
}

FW::ProcessCode
FW::Csv::CsvParticleReader::skip(size_t nEvents)
{
  return ProcessCode::SUCCESS;
}

FW::ProcessCode
FW::Csv::CsvParticleReader::read(FW::AlgorithmContext ctx)
{
  std::string pathIs
      = perEventFilepath(m_cfg.inputDir, m_cfg.inputFileName, ctx.eventNumber);

  FW::CsvReader            pCsvReader(pathIs);
  std::vector<std::string> particleVal;

  ACTS_DEBUG("Assumed particle info in following order: "
             << "'particle_id,vx,vy,vz,px,py,pz,q' in file '"
             << pathIs
             << "'.");

  if (pCsvReader.numPars() < 8) {
    ACTS_ERROR("Number of csv parameters in file '"
               << pathIs
               << "' needs to be at least 8. Aborting.");
    return FW::ProcessCode::ABORT;
  }

  // Create the truth particles
  std::vector<Data::SimParticle> particles;
  while (pCsvReader.readLine(particleVal)) {
    barcode_type   barcode = std::stoul(particleVal[0]);
    Acts::Vector3D vertex  = Acts::Vector3D(std::stof(particleVal[1]),
                                           std::stof(particleVal[2]),
                                           std::stof(particleVal[3]));
    Acts::Vector3D momentum = Acts::Vector3D(std::stof(particleVal[4]),
                                             std::stof(particleVal[5]),
                                             std::stof(particleVal[6]));
    double q = std::stof(particleVal[7]);

    //@TODO: get mass and pdg from config?
    double   mass = 0.;
    pdg_type pdg  = 0;

    ACTS_VERBOSE("particle barcode = " << barcode << " : vertex = ("
                                       << vertex.x()
                                       << ", "
                                       << vertex.y()
                                       << ", "
                                       << vertex.z()
                                       << ")");
    particles.emplace_back(vertex, momentum, mass, q, pdg, barcode);
  }

  // write the truth particles to the EventStore
  if (ctx.eventStore.add(m_cfg.outputParticleCollection, std::move(particles))
      == FW::ProcessCode::ABORT) {
    return FW::ProcessCode::ABORT;
  }

  return ProcessCode::SUCCESS;
}
