// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Plugins/HepMC/SimParticle.hpp"
#include "HepMC/FourVector.h"
#include "HepMC/GenParticle.h"
#include "HepMC/GenVertex.h"
#include "HepPID/ParticleIDMethods.hh"

namespace FW {

//~ /// @class HepMC3Event
//~ ///
//~ /// This class inherits from HepMC::GenEvent and expands the base class by
//~ /// setter and getter functions that take or return parameters such as vertices
//~ /// or particles as Acts objects. The internal behaviour of HepMC::GenEvent is
//~ /// not changed by this class.
//~ ///
template<>
struct SimulatedParticle<HepMC::GenParticle>
{
	static Acts::ParticleProperties
	particleProperties(const HepMC::GenParticle* particle);
	
	static int
	id(const HepMC::GenParticle* particle);

	static const Acts::ProcessVertex
	productionVertex(const HepMC::GenParticle* particle);
	
	static const Acts::ProcessVertex
	endVertex(const HepMC::GenParticle* particle);
	
	static int
	pdgID(const HepMC::GenParticle* particle);
	
	static Acts::Vector3D
	momentum(const HepMC::GenParticle* particle);
	
	static double
	energy(const HepMC::GenParticle* particle);
	
	static double
	mass(const HepMC::GenParticle* particle);
	
	static double
	charge(const HepMC::GenParticle* particle);
	
	static void
	pdgID(HepMC::GenParticle* particle, const int pid);
	
	static void
	momentum(HepMC::GenParticle* particle, const Acts::Vector3D& mom);
	
	static void
	energy(HepMC::GenParticle* particle, const double energy);
	
	static void
	mass(HepMC::GenParticle* particle, const double mass);
	
private:
	static Acts::ProcessVertex
	processVertex(const HepMC::GenVertex* vertex);
};
}  // FW

