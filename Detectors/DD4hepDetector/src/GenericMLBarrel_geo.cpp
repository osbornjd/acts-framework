// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
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
 Constructor for a cylindrical barrel volume, possibly containing layers and the
 layers possibly containing modules. In Atlas style. This constructor was
 specifically introduced for the TrackML detector.
*/

static Ref_t
create_element(Detector& lcdd, xml_h xml, SensitiveDetector sens)
{
  xml_det_t x_det   = xml;
  string    detName = x_det.nameStr();
  // Make DetElement
  DetElement barrelDetector(detName, x_det.id());
  // add Extension to Detlement for the RecoGeometry
  Acts::ActsExtension::Config volConfig;
  volConfig.isBarrel             = true;
  Acts::ActsExtension* volumeExt = new Acts::ActsExtension(volConfig);
  barrelDetector.addExtension<Acts::IActsExtension>(volumeExt);
  // create Volume
  dd4hep::xml::Dimension x_detDim(x_det.dimensions());
  Volume                 barrelVolume(detName,
                      Tube(x_detDim.rmin(), x_detDim.rmax(), x_detDim.dz()),
                      lcdd.vacuum());  // vacuum at the moment change later
  barrelVolume.setVisAttributes(lcdd, x_detDim.visStr());
  // go trough possible layers
  size_t layerNumber = 0;

  for (xml_coll_t j(xml, _U(layer)); j; ++j) {
    xml_comp_t x_layer     = j;
    double     layerRmin   = x_layer.inner_r();
    double     layerRmax   = x_layer.outer_r();
    double     layerLength = x_layer.z();
    // Create Volume and DetElement for Layer
    string layerName = detName + _toString((int)layerNumber, "layer%d");
    Volume layerVolume(layerName,
                       Tube(layerRmin, layerRmax, layerLength),
                       lcdd.material(x_layer.materialStr()));
    DetElement layerDetector(barrelDetector, layerName, layerNumber);
    // Visualization
    layerVolume.setVisAttributes(lcdd, x_layer.visStr());
    // go trough possible modules
    if (x_layer.hasChild(_U(module))) {
      // the module describing the module dimensions
      xml_comp_t x_module         = x_layer.child(_U(module));
      double     modHalfLength    = x_module.length();
      double     modHalfWidth     = x_module.width();
      double     modHalfThickness = x_module.thickness();
      // informations for the placement and tilt in phi
      int    phiRepeat = x_module.repeat();
      double deltaphi  = 2. * M_PI / phiRepeat;
      double phiTilt   = x_module.phi_tilt();
      // informarions for he placement in z
      xml_comp_t x_slice  = x_layer.child(_U(slice));
      int        zRepeat  = x_slice.repeat();
      double     zOverlap = x_slice.dz();
      double     rOffset  = x_slice.offset();  // half offset

      // The placement loop if primarily over z, as it does change
      // - potentially the cooling pipe position due to r staggering along z
      // - the cable support amount

      size_t moduleNumber = 0;
      // Place the Modules in z
      // the distance between the modules in z
      double dz = (2 * modHalfLength - zOverlap);
      // the start value in z
      double startz = -((zRepeat - 1) * 0.5 * dz);

      // place in z
      for (int iz = 0; iz < zRepeat; iz++) {

        // r staggering along z
        double rStaggering = (iz % 2 == 0) ? (0.5 * rOffset) : -(0.5 * rOffset);

        // Create the module volume
        Volume mod_vol("module",
                       Box(modHalfLength, modHalfWidth, modHalfThickness),
                       lcdd.material(x_module.materialStr()));

        // Visualization
        mod_vol.setVisAttributes(lcdd, x_module.visStr());

        //
        // the sensitive placed components to be used later to create the
        // DetElements
        std::vector<PlacedVolume> sensComponents;
        int                       comp_num = 0;
        // go through module components
        for (xml_coll_t comp(x_module, _U(module_component)); comp; ++comp) {
          string     component_name = _toString((int)comp_num, "component%d");
          xml_comp_t x_component    = comp;
          Volume     comp_vol(component_name,
                          Box(x_component.length(),
                              x_component.width(),
                              x_component.thickness()),
                          lcdd.material(x_component.materialStr()));
          comp_vol.setVisAttributes(lcdd, x_component.visStr());

          // make sensitive components sensitive
          if (x_component.isSensitive()) comp_vol.setSensitiveDetector(sens);

          // Place Component in Module
          Position     trans(x_component.x(), 0., x_component.z());
          PlacedVolume placedcomponent = mod_vol.placeVolume(comp_vol, trans);
          placedcomponent.addPhysVolID("component", comp_num);
          if (x_component.isSensitive())
            sensComponents.push_back(placedcomponent);
          comp_num++;
        }
        // add possible trapezoidal shape with hole for cooling pipe
        if (x_module.hasChild(_U(subtraction))) {
          xml_comp_t x_sub          = x_module.child(_U(subtraction));
          xml_comp_t x_trd          = x_sub.child(_U(trd));
          xml_comp_t x_tubs         = x_sub.child(_U(tubs));
          string     component_name = _toString((int)comp_num, "component%d");
          // create the two shapes first
          Trapezoid trap_shape(x_trd.x1(),
                               x_trd.x2(),
                               x_trd.length(),
                               x_trd.length(),
                               x_trd.thickness());
          Tube tubs_shape(x_tubs.rmin(), x_tubs.rmax(), x_tubs.dz());
          // create the substraction
          Volume sub_vol("subtraction_components",
                         SubtractionSolid(trap_shape,
                                          tubs_shape,
                                          Transform3D(RotationX(0.5 * M_PI))),
                         lcdd.material(x_sub.materialStr()));
          sub_vol.setVisAttributes(lcdd, x_sub.visStr());
          // Place the volume in the module
          PlacedVolume placedSub = mod_vol.placeVolume(
              sub_vol,
              Transform3D(RotationZ(0.5 * M_PI) * RotationY(M_PI),
                          Position(0., 0., x_sub.z())));
          placedSub.addPhysVolID("component", comp_num);
          comp_num++;
        }
        // add posibble cooling pipe
        if (x_module.hasChild(_U(tubs))) {
          xml_comp_t x_tubs         = x_module.child(_U(tubs));
          string     component_name = _toString((int)comp_num, "component%d");
          Volume     pipe_vol("CoolingPipe",
                          Tube(x_tubs.rmin(), x_tubs.rmax(), x_tubs.dz()),
                          lcdd.material(x_tubs.materialStr()));
          pipe_vol.setVisAttributes(lcdd, x_tubs.visStr());
          // Place the cooling pipe into the module
          PlacedVolume placedPipe = mod_vol.placeVolume(
              pipe_vol,
              Transform3D(RotationX(0.5 * M_PI) * RotationY(0.5 * M_PI),
                          Position(0., 0., x_tubs.z() - rStaggering)));
          placedPipe.addPhysVolID("component", comp_num);
          comp_num++;
        }

        // create the Acts::DigitizationModule (needed to do geometric
        // digitization) for all modules which have the same segmentation

        auto digiModule
            = FW::DD4hep::rectangleDigiModule(modHalfLength,
                                              modHalfWidth,
                                              modHalfThickness,
                                              sens.readout().segmentation());

        // Visualization
        mod_vol.setVisAttributes(lcdd, x_module.visStr());
        // to be added later to the module name
        string zname = _toString((int)iz, "z%d");
        // the radial position of the module
        double r = ((layerRmax + layerRmin) * 0.5) + rStaggering;
        // current z position
        double z = startz + iz * dz;
        // start phi position
        double minPhi = -M_PI + 0.5 * deltaphi / dd4hep::rad;
        // Place the modules in phi
        for (int iphi = 0; iphi < phiRepeat; ++iphi) {
          // the unique module name
          string module_name = zname + _toString((int)iphi, "module%d");
          // the phi position
          double phi = minPhi + deltaphi * iphi;
          // the position of the module within the layer
          Position trans(r * cos(phi), r * sin(phi), z);
          // Create the module DetElement
          DetElement moduleDetector(layerDetector, module_name, moduleNumber);
          // Set Sensitive Volmes sensitive
          if (x_module.isSensitive()) {
            mod_vol.setSensitiveDetector(sens);
            // create and attach the extension with the shared digitzation
            // module
            Acts::ActsExtension* moduleExtension
                = new Acts::ActsExtension(digiModule);
            moduleDetector.addExtension<Acts::IActsExtension>(moduleExtension);
          }
          // Place Module Box Volumes in layer adding a tilt in phi
          PlacedVolume placedmodule = layerVolume.placeVolume(
              mod_vol,
              Transform3D(RotationY(0.5 * M_PI) * RotationX(-phi - phiTilt),
                          trans));
          placedmodule.addPhysVolID("module", moduleNumber);
          // assign module DetElement to the placed module volume
          moduleDetector.setPlacement(placedmodule);
          ++moduleNumber;
        }
      }
    }
    // todo set granularity of layer material mapping and where material should
    // be mapped
    // hand over modules to ACTS
    Acts::ActsExtension::Config layConfig;
    layConfig.isLayer   = true;
    layConfig.envelopeR = 2. * Acts::units::_mm;
    layConfig.envelopeZ = 2. * Acts::units::_mm;  // maybe change later
    Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
    layerDetector.addExtension<Acts::IActsExtension>(detlayer);
    // Place layer volume
    PlacedVolume placedLayer = barrelVolume.placeVolume(layerVolume);
    placedLayer.addPhysVolID("layer", layerNumber);
    // Assign layer DetElement to layer volume
    layerDetector.setPlacement(placedLayer);
    ++layerNumber;
  }
  // Place Volume
  Volume       mother_vol = lcdd.pickMotherVolume(barrelDetector);
  PlacedVolume placedTube = mother_vol.placeVolume(barrelVolume);
  placedTube.addPhysVolID("system", barrelDetector.id());
  barrelDetector.setPlacement(placedTube);

  return barrelDetector;
}

DECLARE_DETELEMENT(ACTS_GenericMLBarrel, create_element)
