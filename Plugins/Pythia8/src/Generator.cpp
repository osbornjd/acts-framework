// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Pythia8/Generator.hpp"

#include <Acts/Utilities/Units.hpp>
#include "ACTFW/Random/RandomNumbersSvc.hpp"

// wrapper of framework RandomEngine in Pythia8 interface

namespace {

class FrameworkRndmEngine : public Pythia8::RndmEngine
{
public:
  FrameworkRndmEngine(FW::RandomEngine engine) : m_engine(std::move(engine)) {}

  double
  flat()
  {
    return m_engine();
  }

private:
  FW::RandomEngine m_engine;
};
}  // namespace

FW::GPythia8::Generator::Generator(const FW::GPythia8::Generator::Config& cfg,
                                   std::unique_ptr<const Acts::Logger> mlogger)
  : FW::IReaderT<std::vector<Data::SimVertex<>>>()
  , m_cfg(cfg)
  , m_logger(std::move(mlogger))
{
  if (!m_cfg.randomNumberSvc) {
    throw std::invalid_argument("Missing random numbers service");
  }

  // Configure
  for (const auto& pString : m_cfg.processStrings) {
    ACTS_VERBOSE("Setting string " << pString << " to Pythia8");
    m_pythia8.readString(pString.c_str());
  }
  // Set arguments in Settings database.
  m_pythia8.settings.mode("Beams:idA", m_cfg.pdgBeam0);
  m_pythia8.settings.mode("Beams:idB", m_cfg.pdgBeam1);
  m_pythia8.settings.mode("Beams:frameType", 1);
  m_pythia8.settings.parm("Beams:eCM", m_cfg.cmsEnergy);

  // Set the random seed from configuration
  m_pythia8.readString("Random:setSeed = on");
  m_pythia8.init();
}

std::string
FW::GPythia8::Generator::name() const
{
  return "Pythia8Generator";
}

FW::ProcessCode
FW::GPythia8::Generator::read(std::vector<Data::SimVertex<>>& processVertices,
                              size_t                          skip,
                              const FW::AlgorithmContext*     context)
{

  // pythia8 is not thread safe and needs to be protected
  std::lock_guard<std::mutex> lock(m_read_mutex);

  // get the algorithm and event driven random number seed and set it
  // set the seed at initial call
  if (context) {
    auto seed = m_cfg.randomNumberSvc->generateSeed(*context);
    m_pythia8.rndm.init(seed);
    ACTS_DEBUG("Random number seed for Pythia8 set to:" << seed);
  }

  // skip if needed
  if (skip) {
    for (size_t is = 0; is < skip; ++is) m_pythia8.next();
    return FW::ProcessCode::SUCCESS;
  }

  ACTS_VERBOSE("Calling Pythia8 event generation ... ");
  // the actual event
  m_pythia8.next();
  int np = m_pythia8.event.size() - 1;

  ACTS_DEBUG("Pythia8 generated " << np << " particles.");
  // the last vertex
  Acts::Vector3D                 lastVertex(0., 0., 0.);
  std::vector<Data::SimParticle> particlesOut;
  // reserve the maximum amount
  particlesOut.reserve(np);

  // Particle loop
  for (int ip = 0; ip < np; ip++) {
    if (m_pythia8.event[ip].id() == 90) continue;
    if (m_pythia8.event[ip].isFinal()) {
      // get the pdg number
      int pdg = m_pythia8.event[ip].id();
      // pythia returns charge in units of 1/3
      float charge = m_pythia8.particleData.charge(pdg);  // in e
      float mass   = m_pythia8.particleData.m0(pdg);      // in GeV
      // get the momentum
      double px = m_pythia8.event[ip].px();  // [GeV/c]
      double py = m_pythia8.event[ip].py();  // [GeV/c]
      double pz = m_pythia8.event[ip].pz();  // [GeV/c]
      // get the vertex
      double         vx = m_pythia8.event[ip].xProd();  // [mm]
      double         vy = m_pythia8.event[ip].yProd();  // [mm]
      double         vz = m_pythia8.event[ip].zProd();  // [mm]
      Acts::Vector3D vertex(vx, vy, vz);
      // flush if vertices are different
      if (vertex != lastVertex && particlesOut.size()) {
        // create the process vertex, push it
        Data::SimVertex<> pVertex(lastVertex, {}, particlesOut);
        processVertices.push_back(pVertex);
        // reset and reserve the particle vector
        particlesOut.clear();
        particlesOut.reserve(np);
      }
      // remember the vertex
      lastVertex = vertex;
      // unit conversion - should be done with Acts::units
      Acts::Vector3D momentum(px, py, pz);
      Acts::Vector3D position(vx, vy, vz);
      // the particle should be ready now
      particlesOut.push_back(
          Data::SimParticle(position, momentum, mass, charge, pdg));
    }  // final state partice
  }    // particle loop

  // flush a last time time
  if (particlesOut.size()) {
    // create the process vertex, push it
    Data::SimVertex<> pVertex(lastVertex, {}, particlesOut);
    processVertices.push_back(pVertex);
  }
  // return success
  return FW::ProcessCode::SUCCESS;
}
