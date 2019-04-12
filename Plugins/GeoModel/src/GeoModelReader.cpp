// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/GeoModelReader.hpp"

#include <QFileInfo>
#include <QString>
#include <iostream>
#include "Acts/Utilities/Definitions.hpp"
#include "GeoModelDBManager/GMDBManager.h"
#include "GeoModelKernel/GeoAccessVolAndSTAction.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoCons.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelRead/ReadGeoModel.h"

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits

GeoPhysVol*
FW::GeoModelReader::makeDebugDetector() const
{
  GeoElement* hydrogen = new GeoElement("Hydrogen", "H", 1, 1);
  GeoElement* oxygen   = new GeoElement("Oxygen", "O", 8, 16);

  // Define materials
  double       densityOfAir = 0.1;
  GeoMaterial* air          = new GeoMaterial("Air Two", densityOfAir);
  air->add(hydrogen, 2. / 3.);
  air->add(oxygen, 1. / 3.);
  air->lock();

  double       densityOfPolystyrene = 0.2;
  GeoMaterial* poly = new GeoMaterial("std::Polystyrene", densityOfPolystyrene);
  poly->add(hydrogen, 0.5);
  poly->add(oxygen, 0.5);
  poly->lock();

  // Build world volume
  GeoMaterial*  worldMat = new GeoMaterial("std::Air", densityOfAir);
  const GeoBox* worldBox = new GeoBox(1000 * SYSTEM_OF_UNITS::cm,
                                      1000 * SYSTEM_OF_UNITS::cm,
                                      1000 * SYSTEM_OF_UNITS::cm);
  const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
  GeoPhysVol*      world    = new GeoPhysVol(worldLog);

  // Build children
  const GeoBox* toyBox = new GeoBox(800 * SYSTEM_OF_UNITS::cm,
                                    800 * SYSTEM_OF_UNITS::cm,
                                    1000 * SYSTEM_OF_UNITS::cm);
  const GeoLogVol* toyLog  = new GeoLogVol("ToyLog", toyBox, air);
  GeoPhysVol*      toyPhys = new GeoPhysVol(toyLog);

  GeoBox* sPass = new GeoBox(5.0 * SYSTEM_OF_UNITS::cm,
                             30 * SYSTEM_OF_UNITS::cm,
                             30 * SYSTEM_OF_UNITS::cm);
  GeoLogVol*  lPass = new GeoLogVol("Passive", sPass, poly);
  GeoPhysVol* pPass = new GeoPhysVol(lPass);

  GeoBox* sIPass = new GeoBox(4 * SYSTEM_OF_UNITS::cm,
                              25 * SYSTEM_OF_UNITS::cm,
                              25 * SYSTEM_OF_UNITS::cm);
  GeoLogVol*  lIPass = new GeoLogVol("InnerPassive", sIPass, air);
  GeoPhysVol* pIPass = new GeoPhysVol(lIPass);

  // Combine parts
  pPass->add(pIPass);
  toyPhys->add(pPass);

  GeoNameTag* tag = new GeoNameTag("Toy");
  world->add(tag);
  world->add(toyPhys);

  return world;
}

GeoPhysVol*
FW::GeoModelReader::createTheExperiment(GeoPhysVol* world) const
{
  if (world == nullptr) {
    // Setup the 'World' volume from which everything else will be suspended
    double             densityOfAir = 0.1;
    const GeoMaterial* worldMat     = new GeoMaterial("std::Air", densityOfAir);
    const GeoBox*      worldBox     = new GeoBox(1000 * SYSTEM_OF_UNITS::cm,
                                        1000 * SYSTEM_OF_UNITS::cm,
                                        1000 * SYSTEM_OF_UNITS::cm);
    const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
    world                     = new GeoPhysVol(worldLog);
  }
  return world;
}

GeoPhysVol*
FW::GeoModelReader::loadDB(const QString& path) const
{
  GMDBManager* db = new GMDBManager(path);
  // If open, load it
  if (db->isOpen()) {
    GeoModelIO::ReadGeoModel readInGeo = GeoModelIO::ReadGeoModel(db);
    GeoPhysVol*              dbPhys    = readInGeo.buildGeoModel();
    GeoPhysVol*              world     = createTheExperiment(dbPhys);

    return world;
    // else don't
  } else {
    return nullptr;
  }
}

std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelReader::buildPixel(GeoVPhysVol const* bp) const
{
GeoShape const* shape = bp->getLogVol()->getShape();
std::cout << "pixel form: " << shape->type() << std::endl;

  // Walk over all children of the beam pipe volume
  unsigned int nChildren = bp->getNChildVols();
  //~ for (unsigned int i = 0; i < nChildren; i++) {
  for (unsigned int i = 0; i < 1; i++) {
	  GeoVPhysVol const* child = &(*(bp->getChildVol(i)));
	  GeoShape const* cShape = child->getLogVol()->getShape();
	  unsigned int ncChildren = child->getNChildVols();
	std::cout << "child: " << i << "\t" << child->getLogVol()->getName() << "\t" << cShape->type() << "\t" << ncChildren << std::endl;
	for(unsigned int j = 0; j < ncChildren; j++)
	{
		GeoShape const* ccShape = child->getChildVol(j)->getLogVol()->getShape();
		std::cout << "childchild: " << j << "\t" << child->getChildVol(j)->getLogVol()->getName() << "\t" << ccShape->type() << "\t" << child->getChildVol(j)->getNChildVols() << std::endl;
		if(child->getChildVol(j)->getLogVol()->getName() == "Layer0" 
			|| child->getChildVol(j)->getLogVol()->getName() == "Layer1"
			|| child->getChildVol(j)->getLogVol()->getName() == "Layer2"
			|| child->getChildVol(j)->getLogVol()->getName() == "Layer3") // This looks like the pixel layer
		{
			std::cout << child->getChildVol(j)->getLogVol()->getName() << "\t" << dynamic_cast<GeoTube const*>(ccShape)->getRMin()
				<< "\t" << dynamic_cast<GeoTube const*>(ccShape)->getRMax() << "\t" << child->getChildVol(j)->getNChildVols() << std::endl;
			for(unsigned int k = 0; k < child->getChildVol(j)->getNChildVols(); k++)
			{
				GeoVPhysVol const* cchild = &(*(child->getChildVol(j)->getChildVol(k)));
				GeoShape const* cccShape = cchild->getLogVol()->getShape();
				std::cout << "childchildchild: " << k << "\t" << cchild->getLogVol()->getName() << "\t" << cccShape->type() << "\t" << cchild->getNChildVols() << std::endl;
				if(cccShape->type() == "Box")
				{
					GeoBox const* box = dynamic_cast<GeoBox const*>(cccShape);
					std::cout << box->getXHalfLength() << "\t" << box->getYHalfLength() << "\t" << box->getZHalfLength() << std::endl;
				}
			}
		}
	}
  }
  return nullptr;
}
	
std::ostream&
FW::GeoModelReader::treeToStream(GeoVPhysVol const* tree,
                                 std::ostream&      sl) const
{
  // Walk over all children of the current volume
  unsigned int nChildren = tree->getNChildVols();
  for (unsigned int i = 0; i < nChildren; i++) {
    PVConstLink nodeLink = tree->getChildVol(i);
    // Test if it inherits from GeoVPhysVol
    if (dynamic_cast<const GeoVPhysVol*>(&(*(nodeLink)))) {
      const GeoVPhysVol* childVolV = &(*(nodeLink));
      // Test if it is GeoPhysVol
      if (dynamic_cast<const GeoPhysVol*>(childVolV)) {
        // Print content
        const GeoPhysVol* childVol = dynamic_cast<const GeoPhysVol*>(childVolV);
        toStream(childVol, std::cout);
        // Test if it is GeoFullPhysVol
      } else if (dynamic_cast<const GeoFullPhysVol*>(childVolV)) {
        // Print content
        const GeoFullPhysVol* childVol
            = dynamic_cast<const GeoFullPhysVol*>(childVolV);
        toStream(childVol, std::cout);
      }
      // Continue with leaf
      treeToStream(childVolV, sl);

    } else if (dynamic_cast<const GeoNameTag*>(&(*(nodeLink)))) {
      const GeoNameTag* childVol
          = dynamic_cast<const GeoNameTag*>(&(*(nodeLink)));
    }
  }
  return sl;
}

std::ostream&
FW::GeoModelReader::toStream(GeoPhysVol const* gpv, std::ostream& sl) const
{
  sl << "GeoPhysVol" << std::endl;
  sl << gpv->getX().matrix() << std::endl;
  toStream(gpv->getLogVol(), sl);
  return sl;
}

std::ostream&
FW::GeoModelReader::toStream(GeoFullPhysVol const* gfpv, std::ostream& sl) const
{
  sl << "GeoFullPhysVol" << std::endl;
  sl << gfpv->getX().matrix() << std::endl;
  toStream(gfpv->getLogVol(), sl);
  return sl;
}

std::ostream&
FW::GeoModelReader::shapeToStream(GeoShape const* shape, std::ostream& sl) const
{
  sl << "Shape: " << std::endl;
  sl << "\tName: " << shape->type() << " (" << shape->typeID() << ")"
     << std::endl;
  sl << "\tVolume: " << shape->volume() << std::endl;

  if (shape->type() == "Tube") {
    GeoTube const* tube = dynamic_cast<GeoTube const*>(shape);
    sl << "\tRadius: (" << tube->getRMin() << ", " << tube->getRMax() << ")"
       << std::endl;
    sl << "\tHalf length: " << tube->getZHalfLength() << std::endl;
  }
}

std::ostream&
FW::GeoModelReader::toStream(GeoLogVol const* glv, std::ostream& sl) const
{
  sl << "Name: " << glv->getName() << std::endl;

  // Print shape data
  shapeToStream(glv->getShape(), sl);

  // Print material data
  GeoMaterial* geoMaterial = const_cast<GeoMaterial*>(glv->getMaterial());
  sl << "Material: " << std::endl;
  sl << "\tName: " << geoMaterial->getName() << " (" << geoMaterial->getID()
     << ")" << std::endl;

  try {
    geoMaterial->lock();
    sl << "\tClassification numbers: (" << geoMaterial->getRadLength() << ", "
       << geoMaterial->getIntLength() << ", ..., " << geoMaterial->getDensity()
       << ")" << std::endl;
    // Print contained elements of material
    sl << "\tNumber of Elements: " << geoMaterial->getNumElements()
       << std::endl;
    sl << "\tElements: " << std::endl;
    for (unsigned int i = 0; i < geoMaterial->getNumElements(); i++) {
      GeoElement const* geoElement = geoMaterial->getElement(i);
      sl << "\t\tName: " << geoElement->getName() << " ("
         << geoElement->getSymbol() << ") ";
      sl << "A: " << geoElement->getA() << " Z: " << geoElement->getZ();
      sl << " Fraction: " << geoMaterial->getFraction(i) << std::endl;
    }
  } catch (const std::out_of_range& e) {
    sl << "\t\tNo materials available" << std::endl;
  }

  return sl;
}