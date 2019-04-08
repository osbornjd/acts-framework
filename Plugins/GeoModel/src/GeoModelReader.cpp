// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/GeoModelReader.hpp"

#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Tools/CylinderVolumeBuilder.hpp"
#include "Acts/Tools/CylinderVolumeHelper.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Tools/PassiveLayerBuilder.hpp"
#include "Acts/Tools/TrackingVolumeArrayCreator.hpp"
#include "Acts/Utilities/Units.hpp"
#include "GeoModelDBManager/GMDBManager.h"
#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoElement.h"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoMaterial.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelRead/ReadGeoModel.h"
#include "GeoModelKernel/GeoAccessVolAndSTAction.h"
#include <map>
#include <set>
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Surfaces/CylinderBounds.hpp"
#include "Acts/Tools/SurfaceArrayCreator.hpp"
#include "Acts/Layers/CylinderLayer.hpp"

#include <QFileInfo>
#include <QString>

#include <iostream>

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits

GeoPhysVol*
FW::GeoModelReader::makeDebugDetector() const
{
  // TODO: Add elements & lock materials
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
  GeoMaterial*     worldMat = new GeoMaterial("std::Air", densityOfAir);
  const GeoBox*    worldBox = new GeoBox(1000 * SYSTEM_OF_UNITS::cm,
                                      1000 * SYSTEM_OF_UNITS::cm,
                                      1000 * SYSTEM_OF_UNITS::cm);
  const GeoLogVol* worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
  GeoPhysVol*      world    = new GeoPhysVol(worldLog);

  // Build children
  const GeoBox*    toyBox  = new GeoBox(800 * SYSTEM_OF_UNITS::cm,
                                    800 * SYSTEM_OF_UNITS::cm,
                                    1000 * SYSTEM_OF_UNITS::cm);
  const GeoLogVol* toyLog  = new GeoLogVol("ToyLog", toyBox, air);
  GeoPhysVol*      toyPhys = new GeoPhysVol(toyLog);

  GeoBox*     sPass = new GeoBox(5.0 * SYSTEM_OF_UNITS::cm,
                             30 * SYSTEM_OF_UNITS::cm,
                             30 * SYSTEM_OF_UNITS::cm);
  GeoLogVol*  lPass = new GeoLogVol("Passive", sPass, poly);
  GeoPhysVol* pPass = new GeoPhysVol(lPass);

  GeoBox*     sIPass = new GeoBox(4 * SYSTEM_OF_UNITS::cm,
                              25 * SYSTEM_OF_UNITS::cm,
                              25 * SYSTEM_OF_UNITS::cm);
  GeoLogVol*  lIPass = new GeoLogVol("InnerPassive", sIPass, air);
  GeoPhysVol* pIPass = new GeoPhysVol(lIPass);

  pPass->add(pIPass);
  toyPhys->add(pPass);

  GeoNameTag* tag = new GeoNameTag("Toy");
  world->add(tag);
  world->add(toyPhys);

  return world;
}

GeoPhysVol*
FW::GeoModelReader::loadDB(const QString& path) const
{
  GMDBManager* db = new GMDBManager(path);
  if (db->isOpen()) {
    GeoModelIO::ReadGeoModel readInGeo = GeoModelIO::ReadGeoModel(db);

    GeoPhysVol* dbPhys = readInGeo.buildGeoModel();

    GeoPhysVol* world = createTheExperiment(dbPhys);

    return world;
  } else {
    return nullptr;
  }
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
    const GeoLogVol*   worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
    world                       = new GeoPhysVol(worldLog);
  }
  return world;
}

//~ std::shared_ptr<Acts::Surface>
//~ FW::GeoModelReader::createSurface(GeoVPhysVol const* gvpv) const
//~ {
//~ // The surfaces should not have any further children
//~ if (gvpv->getNChildVols() != 0) return nullptr;

//~ // TODO: This might become a switch
//~ // A beam pipe should be a tube
//~ GeoShape const* shape = gvpv->getLogVol()->getShape();
//~ if (shape->type() == "Tube" && dynamic_cast<GeoTube const*>(shape)) {
//~ try {
//~ auto trafo = std::make_shared<const Acts::Transform3D>(gvpv->getX());
//~ GeoTube const* tube = dynamic_cast<GeoTube const*>(shape);
//~ return Acts::Surface::makeShared<Acts::CylinderSurface>(
//~ trafo,
//~ (tube->getRMin() + tube->getRMax()) * 0.5 / SYSTEM_OF_UNITS::mm *
//Acts::units::_mm, ~ tube->getZHalfLength() / SYSTEM_OF_UNITS::mm *
//Acts::units::_mm);
//~ } catch (const std::exception& e) {
//~ std::cout << "Transformation not implemented - returning" << std::endl;
//~ return nullptr;
//~ }
//~ }
//~ return nullptr;
//~ }

//~ std::vector<std::shared_ptr<Acts::Surface>>
//~ FW::GeoModelReader::buildChildrenSurfaces(GeoVPhysVol const* parent) const
//~ {
//~ std::vector<std::shared_ptr<Acts::Surface>> surfaces;

//~ // Walk over all children of the beam pipe volume
//~ unsigned int nChildren = parent->getNChildVols();
//~ for (unsigned int i = 0; i < nChildren; i++) {
//~ if (dynamic_cast<const GeoPhysVol*>(&(*(parent->getChildVol(i))))) {
//~ surfaces.push_back(
//~ std::move(createSurface(&(*(parent->getChildVol(i))))));
//~ }
//~ }
//~ return surfaces;
//~ }

std::array<double, 3>
FW::GeoModelReader::createPassiveSurface(GeoVPhysVol const* gvpv) const
{
  // A beam pipe should be a tube
  GeoShape const* shape = gvpv->getLogVol()->getShape();
  if (shape->type() == "Tube" && dynamic_cast<GeoTube const*>(shape)) {
    GeoTube const* tube = dynamic_cast<GeoTube const*>(shape);
    return {(tube->getRMin() + tube->getRMax()) * 0.5 / SYSTEM_OF_UNITS::mm
                * Acts::units::_mm,
            tube->getZHalfLength() / SYSTEM_OF_UNITS::mm * Acts::units::_mm,
            (tube->getRMax() - tube->getRMin()) / SYSTEM_OF_UNITS::mm
                * Acts::units::_mm};
  }
  // TODO: Get rid of that
  return {0., 0., 0.};
}

void
FW::GeoModelReader::sortAndMergeSurfaces(
    std::vector<std::array<double, 3>>& surfaces) const
{
  // TODO: Merge material
  // Sort surfaces based on the radius
  std::sort(surfaces.begin(),
            surfaces.end(),
            [](std::array<double, 3> a, std::array<double, 3> b) {
              return a[0] < b[0];
            });

  // Test all surfaces with each other
  for (unsigned int i = 0; i < surfaces.size(); i++) {
    for (unsigned int j = i + 1; j < surfaces.size(); j++) {
      // Test for overlapping
      if ((surfaces[i][0] + surfaces[i][2] > surfaces[j][0] - surfaces[j][2])
          || (surfaces[i][0] - surfaces[i][2]
              < surfaces[j][0] + surfaces[j][2])) {
        // Merge surfaces
        surfaces[i][0] = (surfaces[i][0] + surfaces[j][0]) * 0.5;
        surfaces[i][1] = (surfaces[i][1] >= surfaces[j][1]) ? surfaces[i][1]
                                                            : surfaces[j][1];
        surfaces[i][2] = (surfaces[i][2] + surfaces[j][2]) * 0.5;
        // Delete surface
        surfaces.erase(surfaces.begin() + j);
        // Start over (new intersection could appear)
        j = i + 1;
      }
    }
  }
}

std::vector<std::shared_ptr<Acts::TrackingVolume>>
FW::GeoModelReader::buildCentralBeamPipe(GeoVPhysVol const* bp) const
{
  GeoShape const* shape = bp->getLogVol()->getShape();
  if (shape->type() != "Tube" || !dynamic_cast<GeoTube const*>(shape))
    return {};

 std::vector<std::shared_ptr<Acts::TrackingVolume>> result;
	std::vector<std::array<double, 4>> surfaces;
	std::vector<std::shared_ptr<const Acts::Transform3D>> trafos;
	std::set<double> bins;

  // Walk over all children of the beam pipe volume
  unsigned int nChildren = bp->getNChildVols();
  for (unsigned int i = 0; i < nChildren; i++) {
    if (dynamic_cast<const GeoPhysVol*>(&(*(bp->getChildVol(i))))) {
		std::array<double, 3> passSurface = createPassiveSurface(&(*(bp->getChildVol(i)))); // TODO: clean up the mess
      surfaces.push_back({bp->getXToChildVol(i).matrix().transpose().col(3).z(), passSurface[0], passSurface[1], passSurface[2]});
      trafos.push_back(std::make_shared<const Acts::Transform3D>(bp->getXToChildVol(i).matrix().transpose()));
    }
  }
  
	for(auto& s : surfaces)
	{
		  bins.insert(s[0] - s[2]);
		  bins.insert(s[0] + s[2]);
	}

std::vector<std::shared_ptr<const Acts::Surface>> surfaceVector;
double rMin = 100., rMax = 0., zMin = 0., zMax = 0.;

  for(std::set<double>::iterator it = bins.begin(); it != bins.end(); it++)
  {
	  if(*it == 1500)
		break;
		
	  std::set<double>::iterator it2 = std::next(it, 1);
	  double center = (*it2 + *it) * 0.5;
		
	for(unsigned int i = 0; i < surfaces.size(); i++)
	{
		  if(surfaces[i][0] + surfaces[i][2] > center && surfaces[i][0] - surfaces[i][2] < center)
		  {
			surfaceVector.push_back(Acts::Surface::makeShared<Acts::CylinderSurface>(trafos[i], std::make_shared<const Acts::CylinderBounds>(surfaces[i][1], (*it2 - *it) * 0.5)));
			if(surfaces[i][1] > rMax)
				rMax = surfaces[i][1];
			if(surfaces[i][1] < rMin)
				rMin = surfaces[i][1];
			if(trafos[i]->matrix().col(3).z() + (*it2 - *it) * 0.5 > zMax)
				zMax = trafos[i]->matrix().col(3).z() + (*it2 - *it) * 0.5;
			if(trafos[i]->matrix().col(3).z() - (*it2 - *it) * 0.5 < zMin)
				zMin = trafos[i]->matrix().col(3).z() - (*it2 - *it) * 0.5;
		  }
	}
	}

	 Acts::SurfaceArrayCreator surArrCreator;
    std::unique_ptr<Acts::SurfaceArray> surfaceArray = surArrCreator.surfaceArrayOnCylinder(surfaceVector, Acts::BinningType::equidistant, Acts::BinningType::arbitrary);
    
    auto bounds = std::make_shared<const Acts::CylinderBounds>((rMax + rMin) * 0.5, (zMax + zMin) * 0.5);
    auto cylinderLayer = Acts::CylinderLayer::create(std::make_shared<const Acts::Transform3D>(Acts::Transform3D::Identity()), bounds, 
											std::move(surfaceArray), rMax - rMin); 
    	
    //~ GeoTube const* tube = dynamic_cast<GeoTube const*>(shape);
    //~ tube->getRMin()
		// TODO: data needs to received from bp itself
	auto volBounds = std::make_shared<const Acts::CylinderVolumeBounds>(rMin, rMax, (zMax + zMin) * 0.5); // TODO: length == 0.
	auto trackingVolume = Acts::TrackingVolume::create(std::make_shared<const Acts::Transform3D>(Acts::Transform3D::Identity()), volBounds, nullptr, nullptr, {cylinderLayer});
         
         
		//~ result.push_back(cvb.trackingVolume());


  return result;
}

std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelReader::buildFwdBeamPipe(GeoVPhysVol const* bp) const
{
  GeoShape const* shape = bp->getLogVol()->getShape();
  //~ if (shape->type() != "Pcon" || !dynamic_cast<GeoTube const*>(shape))
    //~ return nullptr;

  std::vector<std::array<double, 3>> surfaces;

  // Walk over all children of the beam pipe volume
  unsigned int nChildren = bp->getNChildVols();
  for (unsigned int i = 0; i < nChildren; i++) {
	  std::cout << bp->getXToChildVol(i).matrix().transpose() << std::endl << std::endl;
    //~ if (dynamic_cast<const GeoPhysVol*>(&(*(bp->getChildVol(i))))) {
      //~ surfaces.push_back(
          //~ std::move(createPassiveSurface(&(*(bp->getChildVol(i))))));
    //~ }
  }

  //~ sortAndMergeSurfaces(surfaces);

  //~ Acts::PassiveLayerBuilder::Config plbConfig;
  //~ plbConfig.centralLayerRadii.reserve(surfaces.size());
  //~ plbConfig.centralLayerHalflengthZ.reserve(surfaces.size());
  //~ plbConfig.centralLayerThickness.reserve(surfaces.size());

  //~ for (const std::array<double, 3>& s : surfaces) {
    //~ plbConfig.centralLayerRadii.push_back(s[0]);
    //~ plbConfig.centralLayerHalflengthZ.push_back(s[1]);
    //~ plbConfig.centralLayerThickness.push_back(s[2]);
  //~ }
  //~ auto plb = std::make_shared<Acts::PassiveLayerBuilder>(plbConfig);
  //~ auto lac = std::make_shared<Acts::LayerArrayCreator>();
  //~ auto tvac = std::make_shared<Acts::TrackingVolumeArrayCreator>();

  //~ Acts::CylinderVolumeHelper::Config cvhConfig;
  //~ cvhConfig.layerArrayCreator          = lac;
  //~ cvhConfig.trackingVolumeArrayCreator = tvac;
  //~ auto cvh = std::make_shared<Acts::CylinderVolumeHelper>(cvhConfig);

  //~ GeoTube const* tube = dynamic_cast<GeoTube const*>(shape);

  //~ Acts::CylinderVolumeBuilder::Config cvbConfig;
  //~ cvbConfig.trackingVolumeHelper = cvh;
  //~ cvbConfig.volumeName           = "BeamPipeFwd";
  //~ cvbConfig.buildToRadiusZero = true;
  //~ cvbConfig.layerBuilder      = plb;
  //~ Acts::CylinderVolumeBuilder cvb(cvbConfig);
  //~ return cvb.trackingVolume();
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
  //~ GeoMaterial const* constgeoMaterial = glv->getMaterial();
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
