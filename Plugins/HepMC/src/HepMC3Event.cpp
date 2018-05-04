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
FW::HepMC3Event::genParticleToActs(HepMC::GenParticlePtr genParticle) const
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
