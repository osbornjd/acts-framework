// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
#include "ACTFW/Plugins/GeoModel/GeoModelReader.hpp"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPVLink.h"
#include "GeoModelKernel/GeoNameTag.h"

/// @brief main executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
	FW::GeoModelReader gmr;
	GeoPhysVol* world = gmr.makeDetektor();
	world = gmr.loadDB("/home/user/geometry.db");


	// Walk over all children of the current volume
  unsigned int nChildren = world->getNChildVols();
  for (unsigned int i = 0; i < nChildren; i++) {
	PVConstLink nodeLink = world->getChildVol(i);
	// Test if it inherits from GeoVPhysVol
	if ( dynamic_cast<const GeoVPhysVol*>( &(*( nodeLink ))) ) {
		const GeoVPhysVol *childVolV = &(*( nodeLink ));
		
		if(childVolV->getLogVol()->getName() == "BeamPipeCentral") // + 2x BeamPipeFwd
		{
			gmr.treeToStream(childVolV, std::cout);
			break;
		}
		//~ // Test if it is GeoPhysVol
		//~ if ( dynamic_cast<const GeoPhysVol*>(childVolV) ) {
			//~ // Print content
			//~ const GeoPhysVol* childVol = dynamic_cast<const GeoPhysVol*>(childVolV);
			//~ gmr.toStream(childVol, std::cout);
			//~ // Test if it is GeoFullPhysVol
		//~ } else if ( dynamic_cast<const GeoFullPhysVol*>(childVolV) ) {
			//~ // Print content
			//~ const GeoFullPhysVol* childVol = dynamic_cast<const GeoFullPhysVol*>(childVolV);
			//~ gmr.toStream(childVol, std::cout);
		//~ }
	}
  }
		
	//~ gmr.treeToStream(world, std::cout);
}
