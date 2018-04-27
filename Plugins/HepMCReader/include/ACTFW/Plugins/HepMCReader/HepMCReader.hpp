// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_HEPMCREADER_H
#define ACTFW_PLUGINS_HEPMCREADER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include "HepMC/GenEvent.h"
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Units.hpp"

namespace FW {

//~ /// @class ObjSurfaceWriter
//~ ///
//~ /// An Obj writer for the geometry
//~ ///
class HepMCReader : public HepMC::GenEvent
{
public:

  void readEvt(std::istream& is);

  
  
  //~ const Acts::ProcessVertex
  //~ vertex(unsigned int index);
  
  //~ const std::vector<Acts::ProcessVertex>
  //~ vertex();
  
  //~ const Acts::ParticleProperties
  //~ particle(...
  // particles eines vertex; alle FS particles
  
  
private:
  
  struct EventHead
  {
	int eventNumber;
	int processID;
	int processVertexBarcode;
	double unitMomentum;
	double unitLength;
	// cross section + uncertainty
	std::pair<double, double> crossSection;
	int numVertices;
	int numParticles;
	std::pair<pdg_type, pdg_type> beamParticles = std::make_pair(0, 0); 
	std::vector<long> const* randomStates;
	HepMC::WeightContainer* weights;
	double eventScale;
	double alphaQCD;
	double alphaQED;
  };
  
  struct EventStore
  {
		EventHead evtHead;
		std::vector<Acts::ProcessVertex> vertices;
  };


	std::vector<EventStore> events;
	
	void storeEvent();
	std::vector<Acts::ProcessVertex> storeEventBody();
	EventHead initHead();
	Acts::ParticleProperties buildParticle(HepMC::GenVertex::particles_in_const_iterator it);
}; 

inline void 
FW::HepMCReader::readEvt(std::istream& is) 
{
	read(is);
	storeEvent();
}

inline Acts::ParticleProperties
FW::HepMCReader::buildParticle(HepMC::GenVertex::particles_in_const_iterator it)
{
	return std::move(Acts::ParticleProperties({(*it)->momentum().x(), (*it)->momentum().y(), (*it)->momentum().z()}, 
											  (*it)->generated_mass(),
											  0, //TODO: ladung ist nicht in genparticle hinterlegt -> pid verwenden?
											  (*it)->pdg_id(),
											  (*it)->barcode()));
}



} // FW
#endif  // ACTFW_PLUGINS_HEPMCREADER_H
