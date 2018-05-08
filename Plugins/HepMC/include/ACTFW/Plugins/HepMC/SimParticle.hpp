// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTS/EventData/ParticleDefinitions.hpp"

namespace FW {

//~ /// @class HepMC3Event
//~ ///
//~ /// This class inherits from HepMC::GenEvent and expands the base class by
//~ /// setter and getter functions that take or return parameters such as vertices
//~ /// or particles as Acts objects. The internal behaviour of HepMC::GenEvent is
//~ /// not changed by this class.
//~ ///
template<class T>
struct SimulatedParticle
{};
	
namespace SimParticle{
	
template<class T> static Acts::ParticleProperties
particleProperties(const T* particle)
{
	return SimulatedParticle<T>::particleProperties(particle);
}

template<class T> static int
id(const T* particle)
{
	return SimulatedParticle<T>::id(particle);
}

template<class T> const Acts::ProcessVertex
productionVertex(const T* particle)
{
	return SimulatedParticle<T>::productionVertex(particle);
}

template<class T> const Acts::ProcessVertex
endVertex(const T* particle)
{
	return SimulatedParticle<T>::endVertex(particle);
}

template<class T> int
pdgID(const T* particle)
{
	return SimulatedParticle<T>::pdgID(particle);
}

template<class T> Acts::Vector3D
momentum(const T* particle)
{
	return SimulatedParticle<T>::momentum(particle);
}

template<class T> double
energy(const T* particle)
{
	return SimulatedParticle<T>::energy(particle);
}

template<class T> double
mass(const T* particle)
{
	return SimulatedParticle<T>::mass(particle);
}

template<class T> double
charge(const T* particle)
{
	return SimulatedParticle<T>::charge(particle);
}

template<class T> void
pdgID(T* particle, const int pid)
{
	SimulatedParticle<T>::pdgID(particle, pid);
}

template<class T> void
momentum(T* particle, const Acts::Vector3D& mom)
{
	SimulatedParticle<T>::momentum(particle, mom);
}

template<class T> void
energy(T* particle, const double energy)
{
	SimulatedParticle<T>::momentum(particle, energy);
}
	
template<class T> void
mass(T* particle, const double mass)
{
	SimulatedParticle<T>::mass(particle, mass);
}

}	// SimParticle
}  // FW

