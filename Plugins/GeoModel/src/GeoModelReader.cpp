// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/GeoModelReader.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include "Acts/Layers/ConeLayer.hpp"
#include "Acts/Layers/CylinderLayer.hpp"
#include "Acts/Surfaces/ConeBounds.hpp"
#include "Acts/Surfaces/CylinderBounds.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Tools/PassiveLayerBuilder.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Units.hpp"
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
#include "Acts/Material/BinnedSurfaceMaterial.hpp"

#include <QFileInfo>
#include <QString>

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits

namespace {
double conv = Acts::units::_mm / SYSTEM_OF_UNITS::mm;
}

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
    const GeoLogVol*   worldLog = new GeoLogVol("WorldLog", worldBox, worldMat);
    world                       = new GeoPhysVol(worldLog);
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

double
FW::GeoModelReader::tubeHalfLength(GeoVPhysVol const* gvpv) const
{
  // A beam pipe should be a tube
  GeoShape const* shape = gvpv->getLogVol()->getShape();
  if (shape->type() == "Tube" && dynamic_cast<GeoTube const*>(shape)) {
    GeoTube const* tube = dynamic_cast<GeoTube const*>(shape);
    return tube->getZHalfLength() * conv;
  }
  return 0.;
}

std::set<double>
FW::GeoModelReader::beamPipeMaterialBinning(GeoVPhysVol const* bp) const
{
	std::set<double> bins;

  // Walk over all children of the beam pipe volume
  unsigned int nChildren = bp->getNChildVols();

	if(bp->getLogVol()->getShape()->type() == "Tube")
	{
	  for (unsigned int i = 0; i < nChildren; i++) {
		double hLength = tubeHalfLength(&(*(bp->getChildVol(i))));
		double zShift  = bp->getXToChildVol(i).matrix().transpose()(2, 3);
		// Insert any change of the material along the beam pipe
		bins.insert(zShift - hLength);
		bins.insert(zShift + hLength);
	  }
	  return bins;
	}

	if(bp->getLogVol()->getShape()->type() == "Pcon")
	{
		GeoPcon const* pcon
			= dynamic_cast<GeoPcon const*>(bp->getLogVol()->getShape());
      
		Acts::Transform3D trafo = Acts::Transform3D::Identity();
		
		for (unsigned int i = 0; i < nChildren - 1; i += 2) {
			// Get the begin and end value of z
			double z1    = pcon->getZPlane(i) * conv;
			double z2    = pcon->getZPlane(i + 1) * conv;

			// Transformation of the plane
			trafo.translation()     = Acts::Vector3D(0., 0., z1);
			trafo                   = bp->getX() * trafo;
			bins.insert(trafo.translation().z());
			
			trafo.translation()     = Acts::Vector3D(0., 0., z2);
			trafo                   = bp->getX() * trafo;
			bins.insert(trafo.translation().z());
		}
		return bins;
	}
	return bins;
}
	
std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelReader::buildCentralBeamPipe(GeoVPhysVol const* bp, std::shared_ptr<const Acts::SurfaceMaterial> material) const
{
  // Get the data of the tube
  GeoTube const* tube
      = dynamic_cast<GeoTube const*>(bp->getLogVol()->getShape());
  double rMin        = tube->getRMin() * conv;
  double rMax        = tube->getRMax() * conv;
  double halfLengthZ = tube->getZHalfLength() * conv;

	// TODO: move this out of here
	std::set<double> bins = beamPipeMaterialBinning(bp);

  // Set up lauer
  Acts::PassiveLayerBuilder::Config plbConfig;
  plbConfig.centralLayerRadii.push_back((rMin + rMax) * 0.5);
  plbConfig.centralLayerHalflengthZ.push_back(halfLengthZ);
  plbConfig.centralLayerThickness.push_back(rMax - rMin - Acts::units::_nm);
  plbConfig.centralLayerMaterial.push_back(material);
  Acts::PassiveLayerBuilder plb(plbConfig);

  Acts::LayerArrayCreator                 layArrCreator;
  std::unique_ptr<const Acts::LayerArray> layArray
      = layArrCreator.layerArray(plb.centralLayers(),
                                 rMin,
                                 rMax,
                                 Acts::BinningType::arbitrary,
                                 Acts::BinningValue::binR);

  // Build volume
  auto volBounds = std::make_shared<const Acts::CylinderVolumeBounds>(
      rMin, rMax, halfLengthZ);
  return Acts::TrackingVolume::create(
      std::make_shared<const Acts::Transform3D>(Acts::Transform3D::Identity()),
      volBounds,
      nullptr,
      std::move(layArray));
}

std::vector<std::shared_ptr<const Acts::Layer>>
FW::GeoModelReader::pconLayerVector(const Acts::Transform3D& trafoToVolume, GeoPcon const* pcon, std::pair<double, double>& minMaxZ, std::pair<double, double>& minMaxR) const
{
  // Extract the orientation of the z axis from transformation (matters in the
  // ordering of z values)
  const double zAxisOrientation = trafoToVolume(2, 2);
  
  // Gather layers
  std::vector<std::shared_ptr<const Acts::Layer>> layerVector;
  for (unsigned int i = 0; i < pcon->getNPlanes() - 1; i += 2) {
    // Get the begin and end value of z, inner and outer radius
    double z1    = pcon->getZPlane(i) * conv;
    double z2    = pcon->getZPlane(i + 1) * conv;
    double rMax1 = pcon->getRMaxPlane(i) * conv;
    double rMax2 = pcon->getRMaxPlane(i + 1) * conv;
    double rMin1 = pcon->getRMinPlane(i) * conv;
    double rMin2 = pcon->getRMinPlane(i + 1) * conv;

    // Transformation of the layer
    Acts::Transform3D trafo = Acts::Transform3D::Identity();
    trafo.translation()     = Acts::Vector3D(0., 0., (z2 + z1) * 0.5);
    trafo                   = trafoToVolume * trafo;

    // Bounds of the cone
    double alpha      = std::atan((rMax2 - rMin1) / (z2 - z1));
    auto   coneBounds = std::make_shared<Acts::ConeBounds>(alpha, z1, z2);

    // The layer creation itself
    layerVector.push_back(Acts::ConeLayer::create(
        std::make_shared<const Acts::Transform3D>(trafo),
        coneBounds,
        nullptr,
        z2 - z1 - Acts::units::_nm,
        nullptr,
        Acts::passive));
    
    // TODO: Assign material

    // Search the minimum / maximum value for the envelope cylidner in z ...
    minMaxZ.first  = std::min(minMaxZ.first, z1 * zAxisOrientation);
    minMaxZ.first  = std::min(minMaxZ.first, z2 * zAxisOrientation);
    minMaxZ.second = std::max(minMaxZ.second, z1 * zAxisOrientation);
    minMaxZ.second = std::max(minMaxZ.second, z2 * zAxisOrientation);

    // ... and r
    minMaxR.first  = std::min(minMaxR.first, rMin1);
    minMaxR.first  = std::min(minMaxR.first, rMin2);
    minMaxR.second = std::max(minMaxR.second, rMax1);
    minMaxR.second = std::max(minMaxR.second, rMax2);
  }
}

std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelReader::buildFwdBeamPipe(GeoVPhysVol const* bp, std::shared_ptr<const Acts::SurfaceMaterial> material) const
{
  GeoPcon const* pcon
      = dynamic_cast<GeoPcon const*>(bp->getLogVol()->getShape());

  // Minimum (first) and maximum (second) of the z and radius values
  std::pair<double, double> minMaxZ(std::numeric_limits<double>::max(),
                                    -std::numeric_limits<double>::max());
  std::pair<double, double> minMaxR(std::numeric_limits<double>::max(),
                                    -std::numeric_limits<double>::max());

	// Build the layers
  auto layerVector = pconLayerVector(bp->getX(), pcon, minMaxZ, minMaxR);

  // Put all together into a layer array
  Acts::LayerArrayCreator                 layArrCreator;
  std::unique_ptr<const Acts::LayerArray> layArray
      = layArrCreator.layerArray(layerVector,
                                 minMaxZ.first,
                                 minMaxZ.second,
                                 Acts::BinningType::arbitrary,
                                 Acts::BinningValue::binZ);

  // Build the volume
  auto volBounds = std::make_shared<const Acts::CylinderVolumeBounds>(
      minMaxR.first, minMaxR.second, (minMaxZ.second - minMaxZ.first) * 0.5);
  return Acts::TrackingVolume::create(
      std::make_shared<const Acts::Transform3D>(bp->getX()),
      volBounds,
      nullptr,
      std::move(layArray));
}

std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelReader::buildBeamPipe(GeoVPhysVol const* bp, std::shared_ptr<const Acts::SurfaceMaterial> material) const
{
  // Test the shape
  GeoShape const* shape = bp->getLogVol()->getShape();
  if (shape->type() == "Pcon" && dynamic_cast<GeoPcon const*>(shape))
    return buildFwdBeamPipe(bp);
  if (shape->type() != "Tube" || !dynamic_cast<GeoTube const*>(shape))
    return buildCentralBeamPipe(bp);
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
