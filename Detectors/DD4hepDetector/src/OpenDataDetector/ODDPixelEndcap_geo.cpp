// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Acts/Plugins/DD4hep/ActsExtension.hpp"
#include "DD4hep/DetFactoryHelper.h"
#include "ODDModuleHelper.hpp"

using namespace std;
using namespace dd4hep;

static Ref_t
create_element(Detector& oddd, xml_h xml, SensitiveDetector sens)
{

  xml_det_t x_det   = xml;
  string    detName = x_det.nameStr();

  // Make DetElement
  DetElement endcapDetector(detName, x_det.id());

  // Add Extension to DetElement for the RecoGeometry
  Acts::ActsExtension::Config volConfig;
  volConfig.isEndcap             = true;
  Acts::ActsExtension* detvolume = new Acts::ActsExtension(volConfig);
  endcapDetector.addExtension<Acts::IActsExtension>(detvolume);

  // Make Volume
  dd4hep::xml::Dimension x_det_dim(x_det.dimensions());
  string                 endcapShapeName = x_det_dim.nameStr();

  Tube   endcapShape(x_det_dim.rmin(), x_det_dim.rmax(), x_det_dim.dz());
  Volume endcapVolume(detName, endcapShape, oddd.air());
  endcapVolume.setVisAttributes(oddd, x_det.visStr());

  // Place Volume
  Volume       motherVolume = oddd.pickMotherVolume(endcapDetector);
  Position     translation(0., 0., x_det_dim.z());
  PlacedVolume placedEndcap
      = motherVolume.placeVolume(endcapVolume, translation);

  // Create the module components
  xml_comp_t x_module = x_det.child(_U(module));
  double     ylength  = 0.;
  auto       module = assembleRectangularModule(oddd, sens, x_module, ylength);

  Assembly diskAssembly("disk");

  // DetElement tree
  DetElement diskElementTemplate("DiskElementTemplate", 0);

  // Loop over the rings to create a template disk
  size_t ringNum = 0;
  for (xml_coll_t ring(xml, _U(ring)); ring; ++ring, ++ringNum) {

    // Get the ring
    xml_comp_t x_ring = ring;

    // The ring name
    string   ringName = _toString((int)ringNum, "ring%d");
    Assembly ringAssembly(ringName);
    ringAssembly.setVisAttributes(oddd, x_ring.visStr());

    // DetElement tree
    DetElement ringElement(ringName, ringNum);

    double       r        = x_ring.r();
    double       phi0     = x_ring.phi0();
    unsigned int nModules = x_ring.nphi();
    double       zgap     = x_ring.gap();
    double       phiStep  = 2. * M_PI / nModules;

    // Loop over modules
    for (unsigned int modNum = 0; modNum < nModules; ++modNum) {
      // The module name
      string moduleName = _toString((int)modNum, "module%d");
      // Position it
      double   phi = phi0 + modNum * phiStep;
      double   z   = bool(modNum % 2) ? -zgap : zgap;
      Position trans(r * cos(phi), r * sin(phi), z);
      // Place Module Box Volumes, flip if necessary
      double       flip         = x_det_dim.z() < 0. ? M_PI : 0.;
      PlacedVolume placedModule = ringAssembly.placeVolume(
          module.first,
          Transform3D(RotationZ(phi + 1.5 * M_PI) * RotationY(flip), trans));
      placedModule.addPhysVolID("module", modNum);
      // Clone the detector element
      auto moduleElement = module.second.clone(moduleName, modNum);
      moduleElement.setPlacement(placedModule);
      // Assign it as child to the stave template
      ringElement.add(moduleElement);
    }

    // Place Ring assembly into disk
    PlacedVolume placedRing = diskAssembly.placeVolume(
        ringAssembly, Position(0., 0., x_ring.z_offset()));
    placedRing.addPhysVolID("ring", ringNum);
    ringElement.setPlacement(placedRing);
    // Add it to the Disk element template
    diskElementTemplate.add(ringElement);
  }

  xml_comp_t x_support = x_det.child(_U(support));
  // The support shape
  Tube   supportShape(x_support.rmin(), x_support.rmax(), x_support.dz());
  Volume supportVolume(
      "DiskSupport", supportShape, oddd.material(x_support.materialStr()));
  supportVolume.setVisAttributes(oddd, x_support.visStr());
  diskAssembly.placeVolume(supportVolume);

  // Loop over the layers and place the disk
  size_t layNum = 0;
  for (xml_coll_t lay(xml, _U(layer)); lay; ++lay, ++layNum) {
    // Get the layer
    xml_comp_t x_layer = lay;

    // The Layer envelope volume
    string layerName = detName + std::to_string(layNum);
    Volume layerVolume(layerName,
                       Tube(x_layer.rmin(), x_layer.rmax(), x_layer.dz()),
                       oddd.air());

    layerVolume.setVisAttributes(oddd, x_layer.visStr());

    string diskElName = _toString((int)layNum, "disk%d");

    // The DetElement tree
    DetElement layerElement(layerName, layNum);
    auto       diskElement = diskElementTemplate.clone(diskElName, layNum);

    // Place the disk into the layer
    PlacedVolume placedDisk = layerVolume.placeVolume(diskAssembly);
    diskElement.setPlacement(placedDisk);
    layerElement.add(diskElement);

    // Place Ring assembly into disk
    PlacedVolume placedLayer = endcapVolume.placeVolume(
        layerVolume, Position(0., 0., x_layer.z_offset() - x_det_dim.z()));
    placedLayer.addPhysVolID("layer", layNum);

    // Place the layer with appropriate Acts::Extension
    // Configure the ACTS extension
    Acts::ActsExtension::Config layerConfig;
    layerConfig.isLayer                 = true;
    Acts::ActsExtension* layerExtension = new Acts::ActsExtension(layerConfig);
    layerElement.addExtension<Acts::IActsExtension>(layerExtension);

    // Finish up the DetElement tree
    layerElement.setPlacement(placedLayer);
    endcapDetector.add(layerElement);
  }

  if (x_det.hasChild(_U(disk))) {

    // Endplate disk
    xml_comp_t x_endplate = x_det.child(_U(disk));

    // The Shape and Volume
    Tube   endplateShape(x_endplate.rmin(), x_endplate.rmax(), x_endplate.dz());
    Volume endplateVolume(
        "Endplate", endplateShape, oddd.material(x_endplate.materialStr()));
    endplateVolume.setVisAttributes(oddd, x_endplate.visStr());

    PlacedVolume placedEndplate = endcapVolume.placeVolume(
        endplateVolume,
        Position(0., 0., x_endplate.z_offset() - x_det_dim.z()));
  }

  // "system" is hard coded in the DD4Hep::VolumeManager
  placedEndcap.addPhysVolID("system", endcapDetector.id());
  endcapDetector.setPlacement(placedEndcap);

  // And return it
  return endcapDetector;
}

DECLARE_DETELEMENT(ODDPixelEndcap, create_element)
