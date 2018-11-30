//==========================================================================
//  AIDA Detector description implementation 
//--------------------------------------------------------------------------
// Copyright (C) Organisation europeenne pour la Recherche nucleaire (CERN)
// All rights reserved.
//
// For the licensing terms see $DD4hepINSTALL/LICENSE.
// For the list of contributors see $DD4hepINSTALL/doc/CREDITS.
//
// Author     : M.Frank
//
//==========================================================================
//
// Specialized generic detector constructor
// 
//==========================================================================
#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include "Acts/Plugins/DD4hep/ActsExtension.hpp"
#include "Acts/Plugins/DD4hep/IActsExtension.hpp"

using namespace std;
using namespace dd4hep;
using namespace dd4hep::detail;

static Ref_t create_element(Detector& description, xml_h e, SensitiveDetector sens)  {
  xml_det_t     x_det     = e;
  Material      air       = description.air();
  int           det_id    = x_det.id();
  string        det_name  = x_det.nameStr();
  xml_comp_t    x_dim     = x_det.dimensions();
  DetElement    sdet      (det_name,det_id);

  Acts::ActsExtension::Config volConfig;
  volConfig.isBarrel           = true;
  Acts::ActsExtension* detvolume = new Acts::ActsExtension(volConfig);
  sdet.addExtension<Acts::IActsExtension>(detvolume);

  Volume        motherVol = description.pickMotherVolume(sdet);
  dd4hep::Tube   envShape(x_dim.rmin(), x_dim.rmax(), x_dim.z());
  Volume        envelope  (det_name, envShape, air);
  PlacedVolume  envPhys   = motherVol.placeVolume(envelope,RotationZYX(0,0,0));
  envPhys.addPhysVolID("system", det_id);
  envPhys.addPhysVolID("barrel", 0);
  sdet.setPlacement(envPhys);
  sdet.setVisAttributes(description, x_det.visStr(), envelope);

  sens.setType("calorimeter");
    // Loop over the sets of layer elements in the detector.
    int l_num = 1;
    for(xml_coll_t li(x_det,_U(detector)); li; ++li)  {
      xml_comp_t x_layer = li;
		dd4hep::Tube   layerShape(x_layer.rmin(), x_layer.rmax(), x_layer.dz());
		std::string    layerName = dd4hep::xml::_toString(l_num, "layer%d");
		dd4hep::Volume layerVolume(layerName, layerShape, description.material(x_layer.materialStr()));
		// Create layer detector element
		dd4hep::DetElement lay_det(sdet, layerName, l_num);
  
		if (x_layer.hasAttr(_U(vis))) {
		  layerVolume.setVisAttributes(description, x_layer.visStr());
		}
		if (x_layer.hasAttr(_U(sensitive))) {
		  layerVolume.setSensitiveDetector(sens);
		}
		// Set Acts Extension
		Acts::ActsExtension::Config layConfig;
		layConfig.isLayer             = true;
		Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
		lay_det.addExtension<Acts::IActsExtension>(detlayer);

		// place the layer into the mother volume with a possible translation
		dd4hep::Position     transLayer(0., 0., x_layer.z_offset());
		dd4hep::PlacedVolume placedLayerVolume
			= envelope.placeVolume(layerVolume, transLayer);
		// set volume ID
		placedLayerVolume.addPhysVolID("layer", l_num);
		lay_det.setPlacement(placedLayerVolume);
		l_num++;
    }

  return sdet;
}

DECLARE_DETELEMENT(ACTS_CaloBarrel, create_element)
DECLARE_DEPRECATED_DETELEMENT(CaloBarrel, create_element)
