// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <fstream>
#include <iostream>
#include <mutex>
#include "ACTS/EventData/ParticleDefinitions.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "HepMC/GenEvent.h"
#include "HepMC/GenVertex.h"
#include "HepMC/Reader.h"
#include "HepPID/ParticleIDMethods.hh"
#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"
#include "HepMC/ReaderAscii.h"

namespace FW {

/// @class HepMC3Run
///
/// This class reads a HepMC3 file (run) and stores it in the object.
///
class HepMC3Run
{
public:
	HepMC3Run(const std::string filename);
	
	void 
	readRun();

	unsigned int 
	nEvents();
	
	const std::shared_ptr<HepMC3Event>&
	event(unsigned int index);
	
	const std::vector<std::shared_ptr<HepMC3Event>>&
	events();
	
	bool 
	readEvent();
	
private:
	HepMC::ReaderAscii m_reader;
	std::vector<std::shared_ptr<HepMC3Event>> m_events;
	

};
}  // FW
