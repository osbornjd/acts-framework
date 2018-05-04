// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Run.hpp"
#include <ios>
#include <iostream>
#include <stdexcept>
#include "HepMC/GenParticle.h"


FW::HepMC3Run::HepMC3Run(const std::string filename) : m_reader(filename) {}

void 
FW::HepMC3Run::readRun()
{
	while(readEvent()) {}
}

unsigned int 
FW::HepMC3Run::nEvents()
{
	return m_events.size();
}

const std::shared_ptr<FW::HepMC3Event>&
FW::HepMC3Run::event(unsigned int index)
{
	return m_events.at(index);
}

const std::vector<std::shared_ptr<FW::HepMC3Event>>&
FW::HepMC3Run::events()
{
	return m_events;
}

bool 
FW::HepMC3Run::readEvent()
{
	FW::HepMC3Event hepmc3evt;
	if(m_reader.read_event(hepmc3evt))
	{
		m_events.push_back(std::make_shared<FW::HepMC3Event>(hepmc3evt));
		return true;
	}
	return false;
}
