// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/Generator.hpp"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"

#include <iostream>

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits // so we will get, e.g., 'GeoModelKernelUnits::cm'

GeoPhysVol*
FW::GeoModelReader::makeDetektor()
{
 //-----------------------------------------------------------------------------------//
	// Get the materials that we shall use.                                              //
	// ----------------------------------------------------------------------------------//

	// Bogus densities.  Later: read from database.
	double densityOfAir=0.1, densityOfPolystyrene=0.2;
	const GeoMaterial *air        = new GeoMaterial("Air Two",densityOfAir);
	const GeoMaterial *poly       = new GeoMaterial("std::Polystyrene",densityOfPolystyrene);

  //-----------------------------------------------------------------------------------//
  // create the world volume container and
  // get the 'world' volume, i.e. the root volume of the GeoModel tree
  std::cout << "Creating the 'world' volume, i.e. the root volume of the GeoModel tree..." << std::endl;
  const GeoMaterial* worldMat = new GeoMaterial("std::Air", densityOfAir);
  const GeoBox* worldBox = new GeoBox(1000*SYSTEM_OF_UNITS::cm, 1000*SYSTEM_OF_UNITS::cm, 1000*SYSTEM_OF_UNITS::cm);
  const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
  GeoPhysVol* world = new GeoPhysVol(worldLog);


	//-----------------------------------------------------------------------------------//
	// Next make the box that describes the shape of the toy volume:                     //
	const GeoBox      *toyBox    = new GeoBox(800*SYSTEM_OF_UNITS::cm, 800*SYSTEM_OF_UNITS::cm, 1000*SYSTEM_OF_UNITS::cm);                   //
	// Bundle this with a material into a logical volume:                                //
	const GeoLogVol   *toyLog    = new GeoLogVol("ToyLog", toyBox, air);                 //
	// ..And create a physical volume:                                                   //
	GeoPhysVol        *toyPhys   = new GeoPhysVol(toyLog);                               //

	GeoBox       *sPass = new GeoBox(5.0*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm);
	GeoLogVol    *lPass = new GeoLogVol("Passive", sPass, poly);
	GeoPhysVol   *pPass = new GeoPhysVol(lPass);

	GeoBox       *sIPass = new GeoBox(4*SYSTEM_OF_UNITS::cm, 25*SYSTEM_OF_UNITS::cm, 25*SYSTEM_OF_UNITS::cm);
	GeoLogVol    *lIPass = new GeoLogVol("InnerPassive", sIPass, air);
	GeoPhysVol   *pIPass = new GeoPhysVol(lIPass);

	pPass->add(pIPass);
	toyPhys->add(pPass);

	//------------------------------------------------------------------------------------//
	// Now insert all of this into the world...                                           //
	GeoNameTag *tag = new GeoNameTag("Toy");                                              //
	world->add(tag);                                                                      //
	world->add(toyPhys);                                                                  //
	//------------------------------------------------------------------------------------//




  // --- Now, we test the newly created Geometry

  // get the 'world' GeoLogVol
  std::cout << "\nGetting the GeoLogVol used by the 'world' volume..." << std::endl;
  const GeoLogVol* logVol = world->getLogVol();
  std::cout << "'world' GeoLogVol name: " << logVol->getName() << std::endl;
  std::cout << "'world' GeoMaterial name: " << logVol->getMaterial()->getName() << std::endl;

  // get number of children volumes
  unsigned int nChil = world->getNChildVols();
  std:: cout << "'world' number of children: " << nChil << std::endl;

  // loop over all child nodes
  std::cout << "Looping over all world's 'volume' children (i.e., GeoPhysVol and GeoFullPhysVol)..." << std::endl;
  for (unsigned int idx=0; idx<nChil; ++idx) {
  	PVConstLink nodeLink = world->getChildVol(idx);

  	if ( dynamic_cast<const GeoVPhysVol*>( &(*( nodeLink ))) ) {
  		std::cout << "\t" << "the child n. " << idx << " ";
  		const GeoVPhysVol *childVolV = &(*( nodeLink ));
  		if ( dynamic_cast<const GeoPhysVol*>(childVolV) ) {
  			const GeoPhysVol* childVol = dynamic_cast<const GeoPhysVol*>(childVolV);
  			std::cout << "is a GeoPhysVol, whose GeoLogVol name is: " << childVol->getLogVol()->getName();
  			std::cout<< " and it has  "<<childVol->getNChildVols()<<" child volumes" << std::endl;
  		} else if ( dynamic_cast<const GeoFullPhysVol*>(childVolV) ) {
  			const GeoFullPhysVol* childVol = dynamic_cast<const GeoFullPhysVol*>(childVolV);
  			std::cout << "is a GeoFullPhysVol, whose GeoLogVol name is: " << childVol->getLogVol()->getName();
  			std::cout<< " and it has  "<<childVol->getNChildVols()<<" child volumes" << std::endl;
  		}
    } else if ( dynamic_cast<const GeoNameTag*>( &(*( nodeLink ))) ) {
  		std::cout << "\t" << "the child n. " << idx << " is a GeoNameTag" << std::endl;
  		const GeoNameTag *childVol = dynamic_cast<const GeoNameTag*>(&(*( nodeLink )));
  		std::cout << "\t\tGeoNameTag's name: " << childVol->getName() << std::endl;
    }
  }
  std::cout << "Done." << std::endl;
  return world;	
}
	