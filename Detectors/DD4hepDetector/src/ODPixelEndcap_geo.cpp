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
 Constructor for a disc two like endcap volumes, possibly containing layers and
 the layers possibly containing modules. Both endcaps, the positive and negative
 can be build with this constructor. Atlas like style. This constructor was
 specifically introduced for the TrackML detector, building both the endcap on
 the negative and the positive side.
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
  std::vector<double> sensitiveLayerZ;
  
  for (xml_coll_t j(xml, _U(layer)); j; ++j) {
    xml_comp_t x_layer = j;
    // get the dimensions of the layer;
    double layerRmin    = x_layer.inner_r();
    double layerRmax    = x_layer.outer_r();
    double layerLength  = x_layer.dz();
    double ringStaggerZ = x_layer.z_offset();
    // Create Volume and DetElement for Layer
    string layerName = detName + _toString((int)layerNumber, "layer%d");
    Volume layerVolume(layerName,
                       Tube(layerRmin, layerRmax, layerLength),
                       lcdd.material(x_layer.materialStr()));
    // Create the layer Detector                   
    DetElement layerDetector(posEndcapDetector, layerName, layerNumber);
    // Visualization
    layerVolume.setVisAttributes(lcdd, x_layer.visStr());
    int radialModuleCounter = 0;


    // The module has to be there to build a layer with sensitive
    if (x_layer.hasChild(_U(module))){
        // Get the module
        xml_comp_t x_module = x_layer.child(_U(module));
    
        // Create the module assembly 
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
                            Box(0.5 * x_component.width(),
                                0.5 * x_component.length(),
                                0.5 * x_component.thickness()),
                            lcdd.material(x_component.materialStr()));
          compVolume.setVisAttributes(lcdd, x_component.visStr());

          // Make sensitive components sensitive
          if (x_component.isSensitive()) {
            sensitiveLayerZ.push_back(x_layer.z());
            compVolume.setSensitiveDetector(sens);
          }

          // Place Component in Module
          Position     trans(x_component.x(), x_component.y(), x_component.z());
          PlacedVolume placedComponent
              = moduleAssembly.placeVolume(compVolume, trans);
          // placedComponent.addPhysVolID("component", compNumber);
          if (x_component.isSensitive()) {
            sensComponents.push_back(placedComponent);
          }
          compNumber++;
        }        
    
        std::string mname = "module_r";
        int iring = 0;
    
        // Go trough possible rings
        if (x_layer.hasChild(_U(ring))) {
        
            // Radial rings within the layer
            for (xml_coll_t i(x_layer, _U(ring)); i; i++){
                
                mname += std::to_string(iring++);
                mname += "_m";
                
                // The ring
                xml_comp_t x_ring = i;            
                // Information for placement in phi
                int    nModules = x_ring.nmodules();
                double deltaphi  = 2. * M_PI / nModules;
                double minPhi    = -M_PI + 0.5 * deltaphi;
                // Information for placement in r
                double radius   = x_ring.radius();           
                double zDelta   = x_ring.attr<double>("deltaZ");
                double zOffset  = x_ring.attr<double>("offsetZ");  
                // loop over modules and place them
                for (size_t imod = 0; imod < nModules; ++imod){
                    // unique name 
                    mname += std::to_string(imod);
                    // The phi position of the module
                    double phi = minPhi + deltaphi / dd4hep::rad * imod;
                    // Z position (alterning stagger for subsequent modules)
                    double zPos = (imod % 2) ? (zDelta - 0.5 * zOffset)
                                             : (zDelta + 0.5 * zOffset);
                 
                    Position translation(radius * cos(phi), radius * sin(phi), zPos);
                 
                    // Place Module assemply Volumes in layer
                    Transform3D transform(RotationZ(phi), translation);
                    // Create the module DetElement
                    DetElement moduleDetector(layerDetector,
                                              mname,
                                              iring * nModules + imod);
                    
                    PlacedVolume placedModule
                        = layerVolume.placeVolume(moduleAssembly, transform);
                    placedModule.addPhysVolID(
                        "module", iring * nModules + imod);
                    // assign module DetElement to the placed module volume
                    moduleDetector.setPlacement(placedModule);
                    
                    // Set the cooling pipes
                    if (x_ring.hasChild(_U(tubs))){
                                                
                        xml_comp_t x_tubs     = x_ring.child(_U(tubs));
                        translation = Position(radius * cos(phi), radius * sin(phi), zPos+x_tubs.z());

                        Volume pipeVolume("CoolingPipe",
                                          Tube(x_tubs.rmin(), x_tubs.rmax(), 0.5*x_tubs.length()),
                                          lcdd.material(x_tubs.materialStr()));
                        pipeVolume.setVisAttributes(lcdd, x_tubs.visStr());
                        // Place the cooling pipe into the module
                        PlacedVolume placedPipe = layerVolume.placeVolume(
                            pipeVolume,
                            Transform3D(RotationZ(phi) * RotationY(0.5 * M_PI),
                                        translation));
                        
                    }
                }
            }
        }        
        
        // Support rings
        if (x_layer.hasChild(_Unicode(support_ring))){
            
            // Radial rings within the layer
            for (xml_coll_t i(x_layer, _Unicode(support_ring)); i; i++){
            
                /// Get the support ring 
                xml_comp_t x_support_ring = i;
                double r  = x_support_ring.radius();
                double dR = x_support_ring.thickness(); 
                double dZ = x_support_ring.length();
                
                double rMin = r-0.5*dR;
                double rMax = r+0.5*dR;
                
                Volume supportVolume("CoolingPipe",
                                  Tube(rMin, rMax, dZ),
                                  lcdd.material(x_support_ring.materialStr()));
                supportVolume.setVisAttributes(lcdd, x_support_ring.visStr());

                // Place the cooling pipe into the module
                PlacedVolume placeSupport = layerVolume.placeVolume(
                    supportVolume, Transform3D(Position(0., 0., x_support_ring.z())));
           
            }
            
        }
        
/*        
      // radial placement of modules
      for (xml_coll_t i(x_layer, _U(module)); i; i++) {
        // The module
        // information for placement in z
        double staggerZ    = x_module.z_offset();
        double subStaggerZ = x_module.dz();
        double zPosRing    = (radialModuleCounter % 2) ? (0.5 * ringStaggerZ)
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
        // go through possible components - currently not used
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

        // Place the modules in phi
        for (int iphi = 0; iphi < phiRepeat; iphi++) {
          // the phi position of the module
          double phi = minPhi + deltaphi / dd4hep::rad * iphi;
          // the unique module name
          string module_name
              = _toString((int)(phiRepeat * radialModuleCounter + moduleNumber),
                          "module%d");
          // the z position of the module (with alterning stagger for subsequent
          // modules)
          double zPos = (iphi % 2) ? (zPosRing - 0.5 * staggerZ)
                                   : (zPosRing + 0.5 * staggerZ);
          // phi stagger affects 0 vs 1, 2 vs 3 ... etc
          // -> only works if it is a %4
          // phi sub stagger affects 2 vs 4, 1 vs 3 etc.
          if (subStaggerZ != 0 && !(phiRepeat % 4)) {
            // switch sides
            if (!(iphi % 4))
              zPos += subStaggerZ;
            else if (!((iphi + 1) % 4))
              zPos -= subStaggerZ;
          }
          // the podition of the module
          Position trans(radius * cos(phi), radius * sin(phi), zPos);
          // Create the module DetElement
          DetElement moduleDetector(layerDetector,
                                    module_name,
                                    phiRepeat * radialModuleCounter
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
          // Place Module assemply Volumes in layer
          Transform3D transf(
              RotationX(0.5 * M_PI) * RotationY(phi + 0.5 * M_PI), trans);
          PlacedVolume placedModule
              = layerVolume.placeVolume(moduleVolume, transf);
          placedModule.addPhysVolID(
              "module", phiRepeat * radialModuleCounter + moduleNumber);
          // assign module DetElement to the placed module volume
          moduleDetector.setPlacement(placedModule);
          ++moduleNumber;
        }
        ++radialModuleCounter;
      }
        */
    }
    
    // set granularity of layer material mapping and where material should be
    // mapped hand over modules to ACTS
    Acts::ActsExtension::Config layConfig;
    layConfig.isLayer             = true;
    layConfig.axes                = "XZy";
    Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
    layerDetector.addExtension<Acts::IActsExtension>(detlayer);
    // Placed Layer Volume
    Position     layerPos(0., 0., x_layer.z());
    PlacedVolume placedLayer
        = posEndcapVolume.placeVolume(layerVolume, layerPos);
    placedLayer.addPhysVolID("layer", layerNumber);
    layerDetector.setPlacement(placedLayer);
    ++layerNumber;
  }
  
  
  // Cooling support for Endcaps and Barrel
  if (xml.hasChild(_Unicode(support_sector))){
      
      // Radial rings within the layer
      for (xml_coll_t s(xml, _Unicode(support_sector)); s; s++){
        
        // The cooling support structure
        xml_comp_t x_support_sector = s;
        double r = x_support_sector.radius();

        // number of tubes and phi steps
        size_t nBundles        = x_support_sector.attr<int>("nbundles");
        // overrule if it is per-disk
        bool perDiskBuilding = bool(x_support_sector.attr<int>("perdisk"));        
        double bundleDeltaPhi  = 2. * M_PI / nBundles;
        double minBundlePhi    = x_support_sector.attr<double>("phimin");
        
             
         // Create the pipes
         if (x_support_sector.hasChild(_Unicode(support_bundle))){
             
             xml_comp_t x_support_bundle = x_support_sector.child(_Unicode(support_bundle));
             size_t nTubes               = x_support_bundle.attr<int>("ntubes");
             // correct when per disk building
             if (perDiskBuilding){
                 nTubes = sensitiveLayerZ.size();
             }
             
             std::cout << "Bundle has " << nTubes << " tubes." << std::endl;
        
             if (x_support_bundle.hasChild(_U(tubs))){
                 // 
                 xml_comp_t x_tubs = x_support_bundle.child(_U(tubs));
              
                 for (size_t ibundle = 0; ibundle < nBundles; ++ibundle){
    
                     // delta Phi from radius
                     double dPhiTube = 2*atan2(1.1*x_tubs.rmax(),r);
                     
                     // loop over the tubes
                     for (size_t itube = 0; itube < nTubes; ++itube){
                         
                         double halfZ     = 0.5*x_tubs.length();
                         double positionZ = x_tubs.z();
                         // correct when per disk building
                         if (perDiskBuilding){
                             halfZ -= sensitiveLayerZ[itube];
                             halfZ *= 0.5;
                             // correct back to original if bigger through correction
                             double ohalfZ = 0.5*x_tubs.length();
                             halfZ = halfZ > ohalfZ ? ohalfZ : halfZ;
                             positionZ  -= halfZ-ohalfZ;
                         }     
                         // The support pipe volume 
                         Volume supportPipeVolume("SupportCoolingPipe",
                                           Tube(x_tubs.rmin(), x_tubs.rmax(), halfZ),
                                           lcdd.material(x_tubs.materialStr()));
                         supportPipeVolume.setVisAttributes(lcdd, x_tubs.visStr());
                         
                         double phi = minBundlePhi + ibundle * bundleDeltaPhi + itube * dPhiTube;;
                         // Place the cooling pipe into the module
                         PlacedVolume placedPipe = posEndcapVolume.placeVolume(
                                 supportPipeVolume,
                                 Transform3D(Position(r*cos(phi),r*sin(phi),positionZ)));
                         
                         
                     }
                 }
             }
           }
    }
  }
  // end of cooling support pipe placement
  
  
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

  return endcapDetector;
}

DECLARE_DETELEMENT(ACTS_ODPixelEndcap, create_element)
