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
	
  const GeoLogVol* logVol = world->getLogVol();

  unsigned int nChil = world->getNChildVols();

  for (unsigned int idx=0; idx<nChil; ++idx) {
  	PVConstLink nodeLink = world->getChildVol(idx);
	// TODO: The casts should become a function
  	if ( dynamic_cast<const GeoVPhysVol*>( &(*( nodeLink ))) ) {
  		const GeoVPhysVol *childVolV = &(*( nodeLink ));
  		if ( dynamic_cast<const GeoPhysVol*>(childVolV) ) {
  			const GeoPhysVol* childVol = dynamic_cast<const GeoPhysVol*>(childVolV);
  			gmr.toStream(childVol, std::cout);
  		} else if ( dynamic_cast<const GeoFullPhysVol*>(childVolV) ) {
  			const GeoFullPhysVol* childVol = dynamic_cast<const GeoFullPhysVol*>(childVolV);
  			gmr.toStream(childVol, std::cout);
  		}
  		
    } else if ( dynamic_cast<const GeoNameTag*>( &(*( nodeLink ))) ) {
  		const GeoNameTag *childVol = dynamic_cast<const GeoNameTag*>(&(*( nodeLink )));
    }
  }
}
