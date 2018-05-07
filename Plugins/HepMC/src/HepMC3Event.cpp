// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"


double 
FW::HepMC3Event::momentum_unit()
{
  return (HepMC::GenEvent::momentum_unit() == HepMC::Units::MomentumUnit::MEV
                              ? Acts::units::_MeV
                              : Acts::units::_GeV);
}

double 
FW::HepMC3Event::length_unit()
{
	return (HepMC::GenEvent::length_unit() == HepMC::Units::LengthUnit::MM ? Acts::units::_mm
                                                       : Acts::units::_cm);
}

Acts::Vector3D 
FW::HepMC3Event::event_pos()
{
	Acts::Vector3D vec;
	vec(0) = HepMC::GenEvent::event_pos().x();
	vec(1) = HepMC::GenEvent::event_pos().y();
	vec(2) = HepMC::GenEvent::event_pos().z();
	return vec;
}

double 
FW::HepMC3Event::event_time()
{
	return HepMC::GenEvent::event_pos().t();
}

Acts::ParticleProperties
FW::HepMC3Event::genParticleToActs(const HepMC::GenParticlePtr& genParticle) const
{
	return Acts::ParticleProperties({genParticle->momentum().x(), genParticle->momentum().y(), genParticle->momentum().z()},
									genParticle->generated_mass(),
									HepPID::charge(genParticle->pdg_id()),
									genParticle->pdg_id(),
									genParticle->id());
}

const std::vector<std::shared_ptr<Acts::ParticleProperties>>
FW::HepMC3Event::particles() const
{
	std::vector<std::shared_ptr<Acts::ParticleProperties>> actsParticles; //TODO: If value is set, it can be stored for easier access
	const std::vector<HepMC::GenParticlePtr> genParticles = HepMC::GenEvent::particles();
	
	for(auto& genParticle : genParticles)
		actsParticles.push_back(std::make_shared<Acts::ParticleProperties>(genParticleToActs(genParticle)));

	return std::move(actsParticles);
}

std::vector<std::shared_ptr<Acts::ParticleProperties>>
FW::HepMC3Event::particles()
{
	std::vector<std::shared_ptr<Acts::ParticleProperties>> actsParticles;
	const std::vector<HepMC::GenParticlePtr> genParticles = HepMC::GenEvent::particles();
	
	for(auto& genParticle : genParticles)
		actsParticles.push_back(std::make_shared<Acts::ParticleProperties>(genParticleToActs(genParticle)));

	return std::move(actsParticles);
}
    
const std::vector<std::shared_ptr<Acts::ProcessVertex>>
FW::HepMC3Event::vertices() const
{
	std::vector<std::shared_ptr<Acts::ProcessVertex>> actsVertices;
	const std::vector<HepMC::GenVertexPtr> genVertices = HepMC::GenEvent::vertices();
	
	for(auto& genVertex : genVertices)
	{
		const std::vector<HepMC::GenParticlePtr> genParticlesIn = genVertex->particles_in();
		std::vector<Acts::ParticleProperties> actsParticlesIn;
		for(auto& genParticle : genParticlesIn)
			actsParticlesIn.push_back(genParticleToActs(genParticle));
		
		const std::vector<HepMC::GenParticlePtr> genParticlesOut = genVertex->particles_out();
		std::vector<Acts::ParticleProperties> actsParticlesOut;
		for(auto& genParticle : genParticlesOut)
			actsParticlesOut.push_back(genParticleToActs(genParticle));
		
		Acts::ProcessVertex vertex({genVertex->position().x(),
                                genVertex->position().y(),
                                genVertex->position().z()},
                                genVertex->position().t(),
                                0,  // TODO: what does process_type?
                                actsParticlesIn,
                                actsParticlesOut);
		actsVertices.push_back(std::make_shared<Acts::ProcessVertex>(vertex));		
	}
	return std::move(actsVertices);
}

std::vector<std::shared_ptr<Acts::ProcessVertex>>
FW::HepMC3Event::vertices()
{
	std::vector<std::shared_ptr<Acts::ProcessVertex>> actsVertices;
	const std::vector<HepMC::GenVertexPtr> genVertices = HepMC::GenEvent::vertices();
	
	for(auto& genVertex : genVertices)
	{
		const std::vector<HepMC::GenParticlePtr> genParticlesIn = genVertex->particles_in();
		std::vector<Acts::ParticleProperties> actsParticlesIn;
		for(auto& genParticle : genParticlesIn)
			actsParticlesIn.push_back(genParticleToActs(genParticle));
		
		const std::vector<HepMC::GenParticlePtr> genParticlesOut = genVertex->particles_out();
		std::vector<Acts::ParticleProperties> actsParticlesOut;
		for(auto& genParticle : genParticlesOut)
			actsParticlesOut.push_back(genParticleToActs(genParticle));
		
		Acts::ProcessVertex vertex({genVertex->position().x(),
                                genVertex->position().y(),
                                genVertex->position().z()},
                                genVertex->position().t(),
                                0,  // TODO: what does process_type?
                                actsParticlesIn,
                                actsParticlesOut);
		actsVertices.push_back(std::make_shared<Acts::ProcessVertex>(vertex));		
	}
	return std::move(actsVertices);
}

const std::vector<std::shared_ptr<Acts::ParticleProperties>>
FW::HepMC3Event::beams() const
{
	std::vector<std::shared_ptr<Acts::ParticleProperties>> actsBeams;
	const std::vector<HepMC::GenParticlePtr> genBeams = HepMC::GenEvent::beams();
	for(auto& genBeam : genBeams)
		actsBeams.push_back(std::make_shared<Acts::ParticleProperties>(genParticleToActs(genBeam)));
	return std::move(actsBeams);
}


void 
FW::HepMC3Event::setUnits(const double newMomentumUnit, const double newLengthUnit)
{
	HepMC::Units::MomentumUnit mom;
	if(newMomentumUnit == Acts::units::_MeV)
		mom = HepMC::Units::MomentumUnit::MEV;
	else
		if(newMomentumUnit == Acts::units::_GeV)
			mom = HepMC::Units::MomentumUnit::GEV;
		else
			std::cout << "Invalid unit of momentum: " << newMomentumUnit << std::endl; 
	HepMC::Units::LengthUnit len;
	if(newLengthUnit == Acts::units::_mm)
		len = HepMC::Units::LengthUnit::MM;
	else
		if(newLengthUnit == Acts::units::_cm)
			len = HepMC::Units::LengthUnit::CM;
		else
			std::cout << "Invalid unit of length: " << newLengthUnit << std::endl;
	HepMC::GenEvent::set_units(mom, len);
}
    

void 
FW::HepMC3Event::shiftPositionBy(const Acts::Vector3D& deltaPos, const double deltaTime)
{
	const HepMC::FourVector vec(deltaPos(0), deltaPos(1), deltaPos(2), deltaTime);
	HepMC::GenEvent::shift_position_by(vec);	
}
   
void 
FW::HepMC3Event::shiftPositionTo(const Acts::Vector3D& deltaPos, const double deltaTime) 
{
	const HepMC::FourVector vec(deltaPos(0), deltaPos(1), deltaPos(2), deltaTime);
	HepMC::GenEvent::shift_position_to(vec);
}

HepMC::GenParticlePtr
FW::HepMC3Event::ActsParticleToGen(const Acts::ParticleProperties& actsParticle, int status) const
{
	Acts::Vector3D mom = actsParticle.momentum();
	double energy = sqrt(actsParticle.mass() * actsParticle.mass() + actsParticle.momentum().dot(actsParticle.momentum()));
	const HepMC::FourVector vec(mom(0), mom(1), mom(2), energy);
	HepMC::GenParticle genParticle(vec, actsParticle.pdgID(), status);
	return std::move(HepMC::SmartPointer<HepMC::GenParticle>(&genParticle));
}
  
void 
FW::HepMC3Event::addParticle(std::shared_ptr<Acts::ParticleProperties>& particle, double mass, int status)
{
	HepMC::GenParticlePtr genParticle = ActsParticleToGen(*particle, status);
	genParticle->set_generated_mass(mass);
	
	HepMC::GenEvent::add_particle(genParticle);
}

HepMC::GenVertexPtr
FW::HepMC3Event::createGenVertex(const std::shared_ptr<Acts::ProcessVertex>& vertex, int statusVtx, int statusIn, int statusOut)
{
	Acts::Vector3D pos = vertex->position();
	const HepMC::FourVector vec(pos(0), pos(1), pos(2), vertex->interactionTime());
	
	HepMC::GenVertex genVertex(vec);
	genVertex.set_status(statusVtx);

	const std::vector<Acts::ParticleProperties> particlesIn = vertex->incomingParticles();
	for(auto& particle : particlesIn)
	{
		HepMC::GenParticlePtr genParticle = ActsParticleToGen(particle, statusIn);
		genVertex.add_particle_in(genParticle);
	}
	const std::vector<Acts::ParticleProperties> particlesOut = vertex->outgoingParticles();
	for(auto& particle : particlesOut)
	{
		HepMC::GenParticlePtr genParticle = ActsParticleToGen(particle, statusOut);
		genVertex.add_particle_out(genParticle);
	}
	return HepMC::SmartPointer<HepMC::GenVertex>(&genVertex);
}

void 
FW::HepMC3Event::addVertex(const std::shared_ptr<Acts::ProcessVertex>& vertex, int statusVtx, int statusIn, int statusOut)
{
	HepMC::GenEvent::add_vertex(createGenVertex(vertex, statusVtx, statusIn, statusOut));
}

void 
FW::HepMC3Event::removeParticle(const std::shared_ptr<Acts::ParticleProperties>& actsParticle)
{
	const std::vector<HepMC::GenParticlePtr> genParticles = HepMC::GenEvent::particles();
	const unsigned long id = actsParticle->barcode();
	for(auto& genParticle : genParticles)
		if(genParticle->id() == id)
		{
			HepMC::GenEvent::remove_particle(genParticle);
			break;
		}	
}

void 
FW::HepMC3Event::removeParticles(const std::vector<std::shared_ptr<Acts::ParticleProperties>>& actsParticles)
{
	for(auto& actsParticle : actsParticles)
		removeParticle(actsParticle);
}

bool
FW::HepMC3Event::compareVertices(const std::shared_ptr<Acts::ProcessVertex>& actsVertex, const HepMC::GenVertexPtr& genVertex)
{
	HepMC::FourVector genVec = genVertex->position();
	if(actsVertex->interactionTime() != genVec.t())
		return false;
	Acts::Vector3D actsVec = actsVertex->position();
	if(actsVec(0) != genVec.x())
		return false;
	if(actsVec(1) != genVec.y())
		return false;
	if(actsVec(2) != genVec.z())
		return false;
	if(actsVertex->incomingParticles().size() != genVertex->particles_in().size())
		return false;
	if(actsVertex->outgoingParticles().size() != genVertex->particles_out().size())
		return false;
	return true;
}

void 
FW::HepMC3Event::removeVertex(const std::shared_ptr<Acts::ProcessVertex>& actsVertex)
{
	const std::vector<HepMC::GenVertexPtr> genVertices = HepMC::GenEvent::vertices();
	for(auto& genVertex : genVertices)
		if(compareVertices(actsVertex, genVertex))
		{
			HepMC::GenEvent::remove_vertex(genVertex);
			break;
		}
}

void 
FW::HepMC3Event::addTree(const std::vector<std::shared_ptr<Acts::ProcessVertex>>& actsVertices, int statusVtx, int statusIn, int statusOut)
{
	std::vector<HepMC::GenVertexPtr> genVertices;
	for(auto& actsVertex : actsVertices)
		genVertices.push_back(createGenVertex(actsVertex, statusVtx, statusIn, statusOut));
	std::vector<HepMC::GenParticlePtr> genParticles;
	for(auto& genVertex : genVertices)
	{
		genParticles.insert(genParticles.end(), genVertex->particles_in().begin(), genVertex->particles_in().end());
		genParticles.insert(genParticles.end(), genVertex->particles_out().begin(), genVertex->particles_out().end());
	}	
	HepMC::GenEvent::add_tree(genParticles);
}
    
