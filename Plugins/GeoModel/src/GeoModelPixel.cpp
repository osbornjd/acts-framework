// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/GeoModelPixel.hpp"

#include <iostream>
#include "Acts/Utilities/Definitions.hpp"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelRead/ReadGeoModel.h"

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits

std::vector<GeoVPhysVol const*>
FW::GeoModelPixel::findLayers(GeoVPhysVol const* vol) const
{
	std::vector<GeoVPhysVol const*> layers;
	
	unsigned int ncChildren = vol->getNChildVols();
	for(unsigned int j = 0; j < ncChildren; j++)
	{
		GeoShape const* ccShape = vol->getChildVol(j)->getLogVol()->getShape();
		//~ std::cout << "childchild: " << j << "\t" << vol->getChildVol(j)->getLogVol()->getName() << "\t" << ccShape->type() << "\t" << vol->getChildVol(j)->getNChildVols() << std::endl;
		if(layerKeys.find(vol->getChildVol(j)->getLogVol()->getName()) != layerKeys.end())
		{
			layers.push_back(&(*(vol->getChildVol(j))));
		
			//~ std::cout << vol->getChildVol(j)->getLogVol()->getName() << "\t" << dynamic_cast<GeoTube const*>(ccShape)->getRMin()
				//~ << "\t" << dynamic_cast<GeoTube const*>(ccShape)->getRMax() << "\t" << vol->getChildVol(j)->getNChildVols() << std::endl;
			for(unsigned int k = 0; k < vol->getChildVol(j)->getNChildVols(); k++)
			{
				GeoVPhysVol const* cchild = &(*(vol->getChildVol(j)->getChildVol(k)));
				GeoShape const* cccShape = cchild->getLogVol()->getShape();
				//~ std::cout << "childchildchild: " << k << "\t" << cchild->getLogVol()->getName() << "\t" << cccShape->type() << "\t" << cchild->getNChildVols() << std::endl;
				
				if(cccShape->type() == "Box")
				{
					GeoBox const* box = dynamic_cast<GeoBox const*>(cccShape);
					std::cout << box->getXHalfLength() << "\t" << box->getYHalfLength() << "\t" << box->getZHalfLength() << "\n" << vol->getChildVol(j)->getXToChildVol(k).matrix() << std::endl;
				}
			}
		
		}
	}
	return layers;
}
	
std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelPixel::buildPixel(GeoVPhysVol const* bp) const
{
GeoShape const* shape = bp->getLogVol()->getShape();
std::cout << "pixel form: " << shape->type() << std::endl;

  // Walk over all children of the beam pipe volume
  unsigned int nChildren = bp->getNChildVols();
  //~ for (unsigned int i = 0; i < nChildren; i++) {
  for (unsigned int i = 0; i < 1; i++) {
	  GeoVPhysVol const* child = &(*(bp->getChildVol(i)));
	  GeoShape const* cShape = child->getLogVol()->getShape();
	std::cout << "child: " << i << "\t" << child->getLogVol()->getName() << "\t" << cShape->type() << "\t" << child->getNChildVols() << std::endl;
	
	std::vector<GeoVPhysVol const*> layers = findLayers(child);
  }
  return nullptr;
}