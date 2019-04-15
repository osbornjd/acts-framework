// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/GeoModel/GeoModelBeamPipe.hpp"

#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include "Acts/Layers/ConeLayer.hpp"
#include "Acts/Layers/CylinderLayer.hpp"
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Surfaces/ConeBounds.hpp"
#include "Acts/Surfaces/CylinderBounds.hpp"
#include "Acts/Surfaces/CylinderSurface.hpp"
#include "Acts/Tools/LayerArrayCreator.hpp"
#include "Acts/Tools/PassiveLayerBuilder.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Units.hpp"
#include "GeoModelKernel/GeoPcon.h"
#include "GeoModelKernel/GeoShape.h"
#include "GeoModelKernel/GeoTube.h"
#include "GeoModelKernel/GeoVPhysVol.h"

// Units
#include "GeoModelKernel/Units.h"
#define SYSTEM_OF_UNITS GeoModelKernelUnits

namespace {
double conv = Acts::units::_mm / SYSTEM_OF_UNITS::mm;
}

double
FW::GeoModelBeamPipe::tubeHalfLength(GeoVPhysVol const* gvpv) const
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
FW::GeoModelBeamPipe::beamPipeMaterialBinning(GeoVPhysVol const* bp) const
{
  std::set<double> bins;

  // Walk over all children of the beam pipe volume
  unsigned int nChildren = bp->getNChildVols();

  if (bp->getLogVol()->getShape()->type() == "Tube") {
    for (unsigned int i = 0; i < nChildren; i++) {
      double hLength = tubeHalfLength(&(*(bp->getChildVol(i))));
      double zShift  = bp->getXToChildVol(i).matrix().transpose()(2, 3);
      // Insert any change of the material along the beam pipe
      bins.insert(zShift - hLength);
      bins.insert(zShift + hLength);
    }
    return bins;
  }

  if (bp->getLogVol()->getShape()->type() == "Pcon") {
    GeoPcon const* pcon
        = dynamic_cast<GeoPcon const*>(bp->getLogVol()->getShape());

    Acts::Transform3D trafo = Acts::Transform3D::Identity();

    for (unsigned int i = 0; i < nChildren - 1; i += 2) {
      // Get the begin and end value of z
      double z1 = pcon->getZPlane(i) * conv;
      double z2 = pcon->getZPlane(i + 1) * conv;

      // Transformation of the plane
      trafo.translation() = Acts::Vector3D(0., 0., z1);
      trafo               = bp->getX() * trafo;
      bins.insert(trafo.translation().z());

      trafo.translation() = Acts::Vector3D(0., 0., z2);
      trafo               = bp->getX() * trafo;
      bins.insert(trafo.translation().z());
    }
    return bins;
  }
  return bins;
}

std::shared_ptr<Acts::TrackingVolume>
FW::GeoModelBeamPipe::buildCentralBeamPipe(
    GeoVPhysVol const*                           bp,
    std::shared_ptr<const Acts::SurfaceMaterial> material) const
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
FW::GeoModelBeamPipe::pconLayerVector(const Acts::Transform3D& trafoToVolume,
                                      GeoPcon const*           pcon,
                                      std::pair<double, double>& minMaxZ,
                                      std::pair<double, double>& minMaxR) const
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
FW::GeoModelBeamPipe::buildFwdBeamPipe(
    GeoVPhysVol const*                           bp,
    std::shared_ptr<const Acts::SurfaceMaterial> material) const
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
FW::GeoModelBeamPipe::buildBeamPipe(
    GeoVPhysVol const*                           bp,
    std::shared_ptr<const Acts::SurfaceMaterial> material) const
{
  // Test the shape
  GeoShape const* shape = bp->getLogVol()->getShape();
  if (shape->type() == "Pcon" && dynamic_cast<GeoPcon const*>(shape))
    return buildFwdBeamPipe(bp);
  if (shape->type() != "Tube" || !dynamic_cast<GeoTube const*>(shape))
    return buildCentralBeamPipe(bp);
  return nullptr;
}