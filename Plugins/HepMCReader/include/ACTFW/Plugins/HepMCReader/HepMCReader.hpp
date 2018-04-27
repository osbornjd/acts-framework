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
  
  
  void readEvt(std::istream& is);

	const EventStore&
	event(unsigned int index);
	
	const std::vector<EventStore>&
	allEvents();
	
	const EventHead&
	eventHead(unsigned int index);
	
	const std::vector<EventHead>
	allEventHeads();
	
  const Acts::ProcessVertex&
  vertex(unsigned int indexEvent, unsigned int indexVertex);
  
  const std::vector<Acts::ProcessVertex>&
  vertices(unsigned int indexEvent);
  
  const std::vector<Acts::ProcessVertex>
  vertices();
    

  
private:
	std::vector<EventStore> events;
	
	void storeEvent();
	std::vector<Acts::ProcessVertex> storeEventBody();
	EventHead initHead();
	Acts::ParticleProperties buildParticle(HepMC::GenVertex::particles_in_const_iterator it);
}; 
} // FW
#endif  // ACTFW_PLUGINS_HEPMCREADER_H
