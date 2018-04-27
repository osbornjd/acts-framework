// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMCReader/HepMCReader.hpp"
#include <ios>
#include <iostream>
#include <stdexcept>

void 
FW::HepMCReader::storeEvent() 
{
	EventStore evtStore;
	evtStore.evtHead = initHead();
	evtStore.vertices = storeEventBody();	
	events.push_back(std::move(evtStore));
}

FW::HepMCReader::EventHead
FW::HepMCReader::initHead()
{
	FW::HepMCReader::EventHead evtHead;
	evtHead.eventNumber = this->event_number();
	evtHead.processID = signal_process_id();
	evtHead.processVertexBarcode = (signal_process_vertex() ? signal_process_vertex()->barcode() : 0);
	evtHead.unitMomentum = (momentum_unit() == HepMC::Units::MomentumUnit::MEV ? Acts::units::_MeV : Acts::units::_GeV);
	evtHead.unitLength = (length_unit() == HepMC::Units::LengthUnit::MM ? Acts::units::_mm : Acts::units::_cm);
	// cross section + uncertainty
	evtHead.crossSection = std::make_pair(cross_section()->cross_section(), cross_section()->cross_section_error());
	evtHead.numVertices = vertices_size(); //TODO: kann uU weg
	evtHead.numParticles = particles_size(); //TODO; kann uU weg
	if(beam_particles().first && beam_particles().second) evtHead.beamParticles = std::make_pair(beam_particles().first->barcode(), beam_particles().second->barcode());
	evtHead.randomStates = &(random_states());
	evtHead.weights = &(weights());
	evtHead.eventScale = event_scale();
	evtHead.alphaQCD = alphaQCD();
	evtHead.alphaQED = alphaQED();
	return std::move(evtHead);
}

std::vector<Acts::ProcessVertex>
FW::HepMCReader::storeEventBody()
{
	std::vector<Acts::ProcessVertex> vertices;
	for (HepMC::GenEvent::vertex_const_iterator vtx = this->vertices_begin(); vtx != this->vertices_end(); ++vtx ) 
	{		
	    std::vector<Acts::ParticleProperties> particlesIn, particlesOut;
	    for(HepMC::GenVertex::particles_in_const_iterator pIn = (*vtx)->particles_in_const_begin(); pIn != (*vtx)->particles_in_const_end(); pIn++)
			particlesIn.push_back(std::move(buildParticle(pIn)));

		for(HepMC::GenVertex::particles_out_const_iterator pOut = (*vtx)->particles_out_const_begin(); pOut != (*vtx)->particles_out_const_end(); pOut++)
	    {
			particlesOut.push_back(std::move(buildParticle(pOut)));
		}
		Acts::ProcessVertex vertex({(*vtx)->position().x(), (*vtx)->position().y(), (*vtx)->position().z()},
										 (*vtx)->position().t(),
										 0, //TODO: was bedeutet process_type?
										 particlesIn,
										 particlesOut);
		vertices.push_back(std::move(vertex));
	}
	return std::move(vertices);
}
