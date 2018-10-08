// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/DD4hepDetector/DD4hepDetectorHelper.hpp"
#include "Acts/Plugins/DD4hep/ActsExtension.hpp"
#include "Acts/Plugins/DD4hep/IActsExtension.hpp"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;

/**
 Constructor for a disc like endcap volume, possibly containing layers and the
 layers possibly containing modules. Both endcaps, the positive and negative can
 be build with this constructor. Atlas like style
 */

static Ref_t
create_element(Detector& lcdd, xml_h xml, SensitiveDetector sens)
{
  xml_det_t x_det   = xml;
  string    detName = x_det.nameStr();
  // Create endcap envelope (has no shape because barrel needs to be inbetween)
  dd4hep::Assembly endcapVolume("endcapEnvelope");
  endcapVolume.setVisAttributes(lcdd.invisible());
  // Create endcap DetElement for positive side
  DetElement posEndcapDetector("pos" + detName, x_det.id());
  // add Extension to Detlement for the RecoGeometry
  Acts::ActsExtension::Config volConfig;
  volConfig.isEndcap             = true;
  Acts::ActsExtension* detvolume = new Acts::ActsExtension(volConfig);
  posEndcapDetector.addExtension<Acts::IActsExtension>(detvolume);
  // make Volume
  dd4hep::xml::Dimension x_detDim(x_det.dimensions());
  Volume                 posEndcapVolume(detName,
                         Tube(x_detDim.rmin(), x_detDim.rmax(), x_detDim.dz()),
                         lcdd.vacuum());  // air at the moment change later
  posEndcapVolume.setVisAttributes(lcdd, x_detDim.visStr());
  // go trough possible layers
  size_t layerNumber = 0;
  for (xml_coll_t j(xml, _U(layer)); j; ++j) {
    xml_comp_t x_layer      = j;
    double     layerRmin    = x_layer.inner_r();
    double     layerRmax    = x_layer.outer_r();
    double     layerLength  = x_layer.dz();
    double     ringStaggerZ = x_layer.z_offset();
    // Create Volume and DetElement for Layer
    string layerName = detName + _toString((int)layerNumber, "layer%d");
    Volume layerVolume(layerName,
                       Tube(layerRmin, layerRmax, layerLength),
                       lcdd.material(x_layer.materialStr()));
    DetElement lay_det(posEndcapDetector, layerName, layerNumber);
    // Visualization
    layerVolume.setVisAttributes(lcdd, x_layer.visStr());
    int radialModuleCounter = 0;
    // go trough possible modules
    if (x_layer.hasChild(_U(module))) {
      for (xml_coll_t i(x_layer, _U(module)); i; i++) {
        // The module
        xml_comp_t x_module    = i;
        int        repeat      = x_module.repeat();
        double     deltaphi    = 2. * M_PI / repeat;
        double     minPhi      = -M_PI + 0.5 * deltaphi;
        double     radius      = x_module.radius();
        double     staggerZ    = x_module.z_offset();
        double     subStaggerZ = x_module.dz();
        double     zPosRing = (radialModuleCounter % 2) ? (0.5 * ringStaggerZ)
                                                    : (-0.5 * ringStaggerZ);
        size_t moduleNumber = 0;

        // Create the module volume
        Volume moduleVolume("module",
                            Trapezoid(x_module.x1(),
                                      x_module.x2(),
                                      x_module.thickness(),
                                      x_module.thickness(),
                                      x_module.length()),
                            lcdd.material(x_module.materialStr()));
        moduleVolume.setVisAttributes(lcdd, x_module.visStr());

        // create the Acts::DigitizationModule (needed to do geometric
        // digitization) for all modules which have digitization module
        auto digiModule
            = FW::DD4hep::trapezoidalDigiModule(x_module.x1(),
                                                x_module.x2(),
                                                x_module.length(),
                                                x_module.thickness(),
                                                sens.readout().segmentation());

        // the sensitive placed components to be used later to create the
        // DetElements
        std::vector<PlacedVolume> sensComponents;
        // the possible digitization module
        std::shared_ptr<const Acts::DigitizationModule> digiComponent = nullptr;
        // go through possible components
        int compNumber = 0;
        for (xml_coll_t comp(x_module, _U(module_component)); comp; comp++) {
          xml_comp_t x_comp = comp;
          // create the component volume
          string compName = _toString((int)compNumber, "component%d")
              + x_comp.materialStr();
          Volume compVolume(compName,
                            Trapezoid(x_comp.x1(),
                                      x_comp.x2(),
                                      x_comp.thickness(),
                                      x_comp.thickness(),
                                      x_comp.length()),
                            lcdd.material(x_comp.materialStr()));
          compVolume.setVisAttributes(lcdd, x_comp.visStr());

          // create the Acts::DigitizationModule (needed to do geometric
          // digitization) for all modules which have the sdigitization
          // compoenent
          digiComponent = FW::DD4hep::trapezoidalDigiModule(
              x_comp.x1(),
              x_comp.x2(),
              x_comp.length(),
              x_comp.thickness(),
              sens.readout().segmentation());

          // Set Sensitive Volumes sensitive
          if (x_comp.isSensitive()) compVolume.setSensitiveDetector(sens);

          // place component in module
          Position     translation(0., x_comp.z(), 0.);
          PlacedVolume placed_comp
              = moduleVolume.placeVolume(compVolume, translation);
          if (x_comp.isSensitive()) sensComponents.push_back(placed_comp);
          placed_comp.addPhysVolID("component", moduleNumber);
          ++compNumber;
        }

        // Place the Modules
        for (int iphi = 0; iphi < repeat; iphi++) {
          string zname = _toString((int)iphi, "z%d");

          double phi         = minPhi + deltaphi / dd4hep::rad * iphi;
          string module_name = zname
              + _toString((int)(repeat * radialModuleCounter + moduleNumber),
                          "module%d");

          double zPos = (iphi % 2) ? (zPosRing - 0.5 * staggerZ)
                                   : (zPosRing + 0.5 * staggerZ);

          if (subStaggerZ != 0 && !(repeat % 4)) {
            if (!(iphi % 4))
              zPos += subStaggerZ;
            else if (!((iphi + 1) % 4))
              zPos -= subStaggerZ;
          }

          Position trans(radius * cos(phi), radius * sin(phi), zPos);
          // Create the module DetElement
          DetElement moduleDetector(lay_det,
                                    module_name,
                                    repeat * radialModuleCounter
                                        + moduleNumber);
          // Set Sensitive Volumes sensitive
          if (x_module.isSensitive()) {
            moduleVolume.setSensitiveDetector(sens);
            // create and attach the extension with the shared digitzation
            // module
            Acts::ActsExtension* moduleExtension
                = new Acts::ActsExtension(digiModule);
            moduleDetector.addExtension<Acts::IActsExtension>(moduleExtension);
          }

          int compNumber = 0;
          for (auto& sensComp : sensComponents) {
            // Create DetElement
            DetElement compDetector(moduleDetector, "component", compNumber);
            // create and attach the extension with the shared digitzation
            // module
            Acts::ActsExtension* moduleExtension
                = new Acts::ActsExtension(digiComponent);
            compDetector.addExtension<Acts::IActsExtension>(moduleExtension);
            compDetector.setPlacement(sensComp);
            compNumber++;
          }
          Rotation3D rotation1(1., 0., 0., 0., 1., 0., 0., 0., 1.);
          // Place Module Box Volumes in layer
          Transform3D transf1(
              RotationX(0.5 * M_PI) * RotationY(phi + 0.5 * M_PI), trans);
          PlacedVolume placedmodule
              = layerVolume.placeVolume(moduleVolume, rotation1 * transf1);
          placedmodule.addPhysVolID(
              "module", repeat * radialModuleCounter + moduleNumber);
          // assign module DetElement to the placed module volume
          moduleDetector.setPlacement(placedmodule);
          ++moduleNumber;
        }
        ++radialModuleCounter;
      }
    }
    // set granularity of layer material mapping and where material should be
    // mapped
    // hand over modules to ACTS
    Acts::ActsExtension::Config layConfig;
    layConfig.isLayer             = true;
    layConfig.axes                = "XZy";
    Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
    lay_det.addExtension<Acts::IActsExtension>(detlayer);
    // Placed Layer Volume
    Position     layerPos(0., 0., x_layer.z());
    PlacedVolume placedLayer
        = posEndcapVolume.placeVolume(layerVolume, layerPos);
    placedLayer.addPhysVolID("layer", layerNumber);
    lay_det.setPlacement(placedLayer);
    ++layerNumber;
  }
  // Place Volume
  // if it is the negative endcap the normal vector needs to point into the
  // outside
  double     endcapZPos = (x_detDim.z() > 0.) ? x_detDim.z() : -x_detDim.z();
  Position   posTranslation(0., 0., endcapZPos);
  Rotation3D posRotation(1., 0., 0., 0., 1., 0., 0., 0., 1.);
  Position   negTranslation(0., 0., -endcapZPos);
  Rotation3D negRotation(1., 0., 0., 0., -1., 0., 0., 0., -1.);
  // Place the two (positive and negative) endcap volumes
  PlacedVolume placedPosEndcap = endcapVolume.placeVolume(
      posEndcapVolume, Transform3D(posRotation, posTranslation));
  placedPosEndcap.addPhysVolID("posneg", 0);
  PlacedVolume placedNegEndcap = endcapVolume.placeVolume(
      posEndcapVolume, Transform3D(negRotation, negTranslation));
  placedPosEndcap.addPhysVolID("posneg", 1);
  // create endcap detector element for negative side
  auto negEndcapDetector = posEndcapDetector.clone("neg" + detName);
  // set the placements of positive and negative endcaps
  posEndcapDetector.setPlacement(placedPosEndcap);
  negEndcapDetector.setPlacement(placedNegEndcap);
  // create envelope detelement
  DetElement endcapDetector(detName, x_det.id());
  endcapDetector.add(posEndcapDetector);
  endcapDetector.add(negEndcapDetector);

  PlacedVolume motherVolume
      = lcdd.pickMotherVolume(endcapDetector).placeVolume(endcapVolume);
  motherVolume.addPhysVolID("system", x_det.id());
  endcapDetector.setPlacement(motherVolume);
  /*  PlacedVolume placedTube
        = mother_vol.placeVolume(endcapVolume, endcap_transform);
    placedTube.addPhysVolID("system", endcapDetector.id());
    endcapDetector.setPlacement(placedTube);*/

  return endcapDetector;
}

DECLARE_DETELEMENT(ACTS_GenericMLEndcaps, create_element)
