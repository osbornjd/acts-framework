// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/GeoModelReader.hpp"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoMaterial.h"

#include <iostream>

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits

GeoPhysVol*
FW::GeoModelReader::makeDetektor()
{
	// TODO: Add elements & lock materials
	// Define materials
	double densityOfAir=0.1;
	GeoMaterial *air        = new GeoMaterial("Air Two", densityOfAir);
	double densityOfPolystyrene=0.2;
	GeoMaterial *poly       = new GeoMaterial("std::Polystyrene", densityOfPolystyrene);

  // Build world volume
  GeoMaterial* worldMat = new GeoMaterial("std::Air", densityOfAir);
  const GeoBox* worldBox = new GeoBox(1000*SYSTEM_OF_UNITS::cm, 1000*SYSTEM_OF_UNITS::cm, 1000*SYSTEM_OF_UNITS::cm);
  const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
  GeoPhysVol* world = new GeoPhysVol(worldLog);

	// Build children
	const GeoBox      *toyBox    = new GeoBox(800*SYSTEM_OF_UNITS::cm, 800*SYSTEM_OF_UNITS::cm, 1000*SYSTEM_OF_UNITS::cm);
	const GeoLogVol   *toyLog    = new GeoLogVol("ToyLog", toyBox, air);
	GeoPhysVol        *toyPhys   = new GeoPhysVol(toyLog);

	GeoBox       *sPass = new GeoBox(5.0*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm, 30*SYSTEM_OF_UNITS::cm);
	GeoLogVol    *lPass = new GeoLogVol("Passive", sPass, poly);
	GeoPhysVol   *pPass = new GeoPhysVol(lPass);

	GeoBox       *sIPass = new GeoBox(4*SYSTEM_OF_UNITS::cm, 25*SYSTEM_OF_UNITS::cm, 25*SYSTEM_OF_UNITS::cm);
	GeoLogVol    *lIPass = new GeoLogVol("InnerPassive", sIPass, air);
	GeoPhysVol   *pIPass = new GeoPhysVol(lIPass);

	pPass->add(pIPass);
	toyPhys->add(pPass);

	GeoNameTag *tag = new GeoNameTag("Toy");                                              
	world->add(tag);                                                                      
	world->add(toyPhys);                                                                  

  return world;	
}

  std::ostream&
  FW::GeoModelReader::toStream(GeoPhysVol const* gpv, std::ostream& sl) const
  {
	  toStream(gpv->getLogVol(), sl);
	  return sl;
  }
  
  std::ostream&
  FW::GeoModelReader::toStream(GeoFullPhysVol const* gfpv, std::ostream& sl) const
  {
	  toStream(gfpv->getLogVol(), sl);
	  return sl;
  }
    
    std::ostream&
    FW::GeoModelReader::toStream(GeoLogVol const* glv, std::ostream& sl) const
    {
		sl << "Name: " << glv->getName() << std::endl;
		GeoShape const* geoShape = glv->getShape();
		sl << "Shape: " << std::endl;
		sl << "\tName: " << geoShape->type() << " (" << geoShape->typeID() << ")" << std::endl;
		sl << "\tVolume: " << geoShape->volume() << std::endl;
		
		GeoMaterial const* geoMaterial = glv->getMaterial();
		sl << "Material: " << std::endl;
		sl << "\tName: " << geoMaterial->getName() << " (" << geoMaterial->getID() << ")" << std::endl;
		//~ sl << "\tClassification numbers: (" << geoMaterial->getRadLength() << ", " << geoMaterial->getIntLength() << ", ..., " << geoMaterial->getDensity() << ")" << std::endl;
		return sl;
	}
	