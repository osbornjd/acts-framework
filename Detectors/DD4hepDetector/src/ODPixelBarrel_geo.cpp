// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
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
 Constructor for a Cylindrical Pixel barrel volume from DD4Hep
 with module sub structure and support

 It is used for the OpenData detector
*/

static Ref_t
create_element(Detector& lcdd, xml_h xml, SensitiveDetector sens)
{
  xml_det_t x_detector = xml;
  string    detName    = x_detector.nameStr();

  // Make DetElement
  DetElement barrelDetector(detName, x_detector.id());
  // add Extension to Detlement for the RecoGeometry
  Acts::ActsExtension::Config volConfig;
  volConfig.isBarrel             = true;
  Acts::ActsExtension* volumeExt = new Acts::ActsExtension(volConfig);
  barrelDetector.addExtension<Acts::IActsExtension>(volumeExt);
  // create Volume
  dd4hep::xml::Dimension x_dimension(x_detector.dimensions());

  double g4tolerance = x_dimension.attr<double>("g4tolerance");

  Volume barrelVolume(
      detName,
      Tube(x_dimension.rmin(), x_dimension.rmax(), 0.5 * x_dimension.length()),
      lcdd.vacuum());
  barrelVolume.setVisAttributes(lcdd, x_dimension.visStr());

  // go trough possible layers
  size_t layerNumber = 0;

  for (xml_coll_t j(xml, _U(layer)); j; ++j) {

    xml_comp_t x_layer        = j;
    double     layerR         = x_layer.r();
    double     layerRmin      = x_layer.rmin();
    double     layerRmax      = x_layer.rmax();
    double     layerLength    = x_layer.length();
    double     layerRenvelope = x_layer.attr<double>("envelopeR");

    // create the layer name
    string layerName = detName + _toString((int)layerNumber, "layer%d");
    // informarions for stave description
    int    nStaves  = x_layer.attr<int>("nstaves");
    double deltaPhi = 2. * M_PI / nStaves;

    if (x_layer.hasChild(_U(stave))) {

      xml_comp_t x_stave  = x_layer.child(_U(stave));
      double     phiTilt  = x_stave.phi_tilt();
      double     nModules = x_stave.nmodules();
      double     zOverlap = x_stave.attr<double>("overlapZ");
      double     rDelta   = x_stave.attr<double>("deltaR");

      // only build the layer if it has modules anyway
      if (x_detector.hasChild(_U(module))) {

        // the module describing the module dimensions
        xml_comp_t x_module = x_detector.child(_U(module));
        double     moduleHl = 0.5 * x_module.length();
        double     moduleHw = 0.5 * x_module.width();
        double     moduleHt = 0.5 * x_module.thickness();

        // Create Assembly and DetElement for Layer
        Assembly layerAssembly(layerName);
        // Visualization
        layerAssembly.setVisAttributes(lcdd, x_layer.visStr());

        DetElement layerDetector(barrelDetector, layerName, layerNumber);

        // The placement loop if primarily over z, as it does change
        // - potentially the cooling pipe position due to r staggering along z
        // - the cable support amount

        size_t moduleNumber = 0;

        // The pixel digitization module
        auto digiModule = FW::DD4hep::rectangleDigiModule(
            moduleHl, moduleHw, moduleHt, sens.readout().segmentation());

        // Placing loop in z
        for (int iz = 0; iz < nModules; iz++) {

          // r staggering along z
          double dr = (iz % 2 == 0) ? (0.5 * rDelta) : -(0.5 * rDelta);

          Assembly moduleAssembly("module");

          // the sensitive placed components to be used later to create the
          // DetElements
          std::vector<PlacedVolume> sensComponents;
          int                       compNumber = 0;
          // go through module components
          for (xml_coll_t comp(x_module, _U(module_component)); comp; ++comp) {
            string component_name  = _toString((int)compNumber, "component%d");
            xml_comp_t x_component = comp;
            Volume     compVolume(component_name,
                              Box(0.5 * x_component.length(),
                                  0.5 * x_component.width(),
                                  0.5 * x_component.thickness()),
                              lcdd.material(x_component.materialStr()));
            compVolume.setVisAttributes(lcdd, x_component.visStr());

            // Make sensitive components sensitive
            if (x_component.isSensitive()) {
              compVolume.setSensitiveDetector(sens);
            }

            // Place Component in Module
            Position     trans(x_component.x(), 0., x_component.z() + dr);
            PlacedVolume placedComponent
                = moduleAssembly.placeVolume(compVolume, trans);
            // placedComponent.addPhysVolID("component", compNumber);
            if (x_component.isSensitive()) {
              sensComponents.push_back(placedComponent);
            }
            compNumber++;
          }
          // Add possible trapezoidal shape with hole for cooling pipe
          if (x_module.hasChild(_U(subtraction))) {
            xml_comp_t x_sub      = x_module.child(_U(subtraction));
            xml_comp_t x_trd      = x_sub.child(_U(trd));
            xml_comp_t x_tubs     = x_sub.child(_U(tubs));
            string component_name = _toString((int)compNumber, "component%d");
            // create the two shapes first
            Trapezoid trapShape(x_trd.x1(),
                                x_trd.x2(),
                                0.5 * x_trd.length(),
                                0.5 * x_trd.length(),
                                x_trd.thickness());
            Tube subsShape(x_tubs.rmin(), x_tubs.rmax(), 0.5 * x_tubs.length());
            // create the substraction
            Transform3D subTransform(RotationX(0.5 * M_PI));

            Volume subVolume(
                "subtraction_components",
                SubtractionSolid(trapShape, subsShape, subTransform),
                lcdd.material(x_sub.materialStr()));
            subVolume.setVisAttributes(lcdd, x_sub.visStr());
            // Place the volume in the module
            PlacedVolume placedSub = moduleAssembly.placeVolume(
                subVolume,
                Transform3D(RotationZ(0.5 * M_PI) * RotationY(M_PI),
                            Position(0., 0., x_sub.z())));
            compNumber++;
          }

          // Adjust pipe and cable length for outermost module rings
          double startz
              = -(nModules * 0.5) * (2 * moduleHl - zOverlap) + moduleHl;
          double stubz = 0.5 * layerLength + startz - moduleHl;

          double addHl = 0.;
          double addX  = 0.;
          if (iz == 0 or iz == nModules - 1) {
            addHl += 0.5 * stubz;
            addX += iz ? -0.5 * stubz : +0.5 * stubz;
          }

          // Add possible cooling pipe
          if (x_module.hasChild(_U(tubs))) {
            xml_comp_t x_tubs     = x_module.child(_U(tubs));
            string component_name = _toString((int)compNumber, "component%d");

            // Pipe half length, modified for first and last
            double pipeHl
                = 0.5 * (x_tubs.length() - zOverlap) - g4tolerance + addHl;
            double pipeX = addX;

            Volume pipeVolume("CoolingPipe",
                              Tube(x_tubs.rmin(), x_tubs.rmax(), pipeHl),
                              lcdd.material(x_tubs.materialStr()));
            pipeVolume.setVisAttributes(lcdd, x_tubs.visStr());
            // Place the cooling pipe into the module
            PlacedVolume placedPipe = moduleAssembly.placeVolume(
                pipeVolume,
                Transform3D(RotationX(0.5 * M_PI) * RotationY(0.5 * M_PI),
                            Position(pipeX, 0., x_tubs.z())));
            compNumber++;
          }

          // Add cable bundle
          if (x_module.hasChild(_Unicode(cable))) {
            // the naximum cable bundle
            int maxCableBundle = floor(nModules * 0.5) - int(nModules) % 2;
            int curCableBundle = maxCableBundle - iz;
            if (curCableBundle < 1) {
              curCableBundle = abs(curCableBundle) + 1;
            }

            xml_comp_t x_cable = x_module.child(_Unicode(cable));

            // Pipe half length, modified for first and last
            double cableHl
                = 0.5 * (x_cable.length() - zOverlap) - g4tolerance + addHl;

            string component_name = _toString((int)compNumber, "component%d");
            Volume cableVolume("Cable",
                               Tube(0., x_cable.rmax(), cableHl),
                               lcdd.material(x_cable.materialStr()));
            cableVolume.setVisAttributes(lcdd, x_cable.visStr());

            // The right amount of cable for the number of volumes
            for (int ic = 0; ic < curCableBundle; ++ic) {
              // Place the cooling pipe into the module
              PlacedVolume placedCable = moduleAssembly.placeVolume(
                  cableVolume,
                  Transform3D(RotationX(0.5 * M_PI) * RotationY(0.5 * M_PI),
                              Position(x_cable.x() + addX,
                                       x_cable.y() - 2 * ic * x_cable.rmax(),
                                       x_cable.z() - ic * x_cable.rmax())));
              compNumber++;
            }
          }

          // Placing loop in phi
          // to be added later to the module name
          string zname = _toString((int)iz, "z%d");

          // current z position
          double z = startz + iz * (2 * moduleHl - zOverlap);

          // start phi position
          double minPhi = -M_PI + 0.5 * deltaPhi / dd4hep::rad;

          // Place the modules in phi
          for (int iphi = 0; iphi < nStaves; ++iphi) {
            // the unique module name
            string module_name = zname + _toString((int)iphi, "module%d");
            // the phi position
            double phi = minPhi + deltaPhi * iphi;
            // the position of the module within the layer
            Position trans(layerR * cos(phi), layerR * sin(phi), z);
            // Create the module DetElement
            DetElement moduleDetector(layerDetector, module_name, moduleNumber);
            // Set Sensitive Volmes sensitive
            if (x_module.isSensitive()) {
              moduleAssembly.setSensitiveDetector(sens);
              // create and attach the extension with the shared digitzation
              // module
              Acts::ActsExtension* moduleExtension
                  = new Acts::ActsExtension(digiModule);
              moduleDetector.addExtension<Acts::IActsExtension>(
                  moduleExtension);
            }
            // Place Module Box Volumes in layer adding a tilt in phi
            PlacedVolume placedModule = layerAssembly.placeVolume(
                moduleAssembly,
                Transform3D(RotationY(0.5 * M_PI) * RotationX(-phi - phiTilt),
                            trans));

            placedModule.addPhysVolID("module", moduleNumber);
            // assign module DetElement to the placed module volume
            moduleDetector.setPlacement(placedModule);
            ++moduleNumber;
          }
        }

        // Create support cylinder
        if (x_layer.hasChild(_Unicode(support))) {
          xml_comp_t x_support = x_layer.child(_Unicode(support));
          // Add cylindrical support
          if (x_support.hasChild(_Unicode(cylinder))) {
            xml_comp_t x_cylinder = x_support.child(_Unicode(cylinder));
            // Add the support volume
            Volume supportCylinder("SupportCylinder",
                                   Tube(layerRmax,
                                        layerRmax + x_cylinder.thickness(),
                                        0.5 * layerLength),
                                   lcdd.material(x_cylinder.materialStr()));
            supportCylinder.setVisAttributes(lcdd, x_cylinder.visStr());
            // Place the cooling pipe into the module
            PlacedVolume placedSupport
                = layerAssembly.placeVolume(supportCylinder);
          }
          // Add ring support
          if (x_support.hasChild(_Unicode(ring))) {
            xml_comp_t x_ring = x_support.child(_Unicode(ring));
            // The ring support
            Tube ring(layerR - 0.5 * x_ring.thickness(),
                      layerR + 0.5 * x_ring.thickness(),
                      0.5 * x_ring.length());
            // Add the support volume
            Volume supportRing(
                "SupportRing", ring, lcdd.material(x_ring.materialStr()));
            supportRing.setVisAttributes(lcdd, x_ring.visStr());

            double zring = 0.5 * (layerLength + x_ring.length()) + g4tolerance;

            // Place the negative ring
            PlacedVolume pacedRingN = layerAssembly.placeVolume(
                supportRing, Transform3D(Position(0., 0., -zring)));
            // Place the positive ring
            PlacedVolume pacedRingP = layerAssembly.placeVolume(
                supportRing, Transform3D(Position(0., 0., +zring)));

            // Add spokes to next layer
            if (x_support.hasChild(_Unicode(spoke))) {
              xml_comp_t x_spoke = x_support.child(_Unicode(spoke));
              int        nSpokes = x_spoke.repeat();
              // The spoke shape
              Tube spoke(layerR + 0.5 * x_ring.thickness() + g4tolerance,
                         x_spoke.rmax() - g4tolerance,
                         0.5 * x_ring.length(),
                         x_spoke.deltaphi());

              // Add the support volume
              Volume supportSpoke(
                  "SupportSpoke", spoke, lcdd.material(x_spoke.materialStr()));
              supportSpoke.setVisAttributes(lcdd, x_spoke.visStr());

              double dPhi = 2 * M_PI / nSpokes;

              for (int isp = 0; isp < nSpokes; ++isp) {
                // the current cPhi status
                double cPhi = isp * dPhi - 0.5 * x_spoke.deltaphi();
                // Place the negative ring
                PlacedVolume placedSpokeN = layerAssembly.placeVolume(
                    supportSpoke,
                    Transform3D(RotationZ(cPhi), Position(0., 0., -zring)));

                // Place the negative ring
                PlacedVolume placedSpokeP = layerAssembly.placeVolume(
                    supportSpoke,
                    Transform3D(RotationZ(cPhi), Position(0., 0., zring)));
              }
            }
          }
        }

        // Place layer volume
        PlacedVolume placedLayer = barrelVolume.placeVolume(layerAssembly);
        placedLayer.addPhysVolID("layer", layerNumber);
        // Assign layer DetElement to layer volume
        layerDetector.setPlacement(placedLayer);
        ++layerNumber;
      }
    }
  }

  // Place Volume
  Volume       motherVolume = lcdd.pickMotherVolume(barrelDetector);
  PlacedVolume placedTube   = motherVolume.placeVolume(barrelVolume);
  placedTube.addPhysVolID("system", barrelDetector.id());
  barrelDetector.setPlacement(placedTube);

  return barrelDetector;

}

DECLARE_DETELEMENT(ACTS_ODPixelBarrel, create_element)
