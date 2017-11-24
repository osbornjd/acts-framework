// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTS/Plugins/DD4hepPlugins/ActsExtension.hpp"
#include "ACTS/Plugins/DD4hepPlugins/IActsExtension.hpp"
#include "DD4hep/DetFactoryHelper.h"

using namespace std;
using namespace dd4hep;

/**
 Constructor for a cylindrical barrel volume, possibly containing layers
*/

static Ref_t
create_element(Detector& lcdd, xml_h xml, SensitiveDetector sens)
{
  xml_det_t x_det    = xml;
  string    det_name = x_det.nameStr();
  // Make DetElement
  DetElement cylinderVolume(det_name, x_det.id());
  // add Extension to Detlement for the RecoGeometry
  Acts::ActsExtension::Config volConfig;
  volConfig.isBarrel             = true;
  Acts::ActsExtension* detvolume = new Acts::ActsExtension(volConfig);
  cylinderVolume.addExtension<Acts::IActsExtension>(detvolume);
  // make Volume
  dd4hep::xml::Dimension x_det_dim(x_det.dimensions());
  Tube   tube_shape(x_det_dim.rmin(), x_det_dim.rmax(), x_det_dim.dz());
  Volume tube_vol(det_name, tube_shape, lcdd.vacuum());
  tube_vol.setVisAttributes(lcdd, x_det_dim.visStr());
  // go trough possible layers
  size_t layer_num = 0;

  for (xml_coll_t j(xml, _U(layer)); j; ++j) {
    xml_comp_t x_layer  = j;
    double     l_rmin   = x_layer.inner_r();
    double     l_rmax   = x_layer.outer_r();
    double     l_length = x_layer.z();
    // Create Volume and DetElement for Layer
    string layer_name = det_name + _toString((int)layer_num, "layer%d");
    Volume layer_vol(layer_name,
                     Tube(l_rmin, l_rmax, l_length),
                     lcdd.material(x_layer.materialStr()));
    DetElement lay_det(cylinderVolume, layer_name, layer_num);
    // Visualization
    layer_vol.setVisAttributes(lcdd, x_layer.visStr());
    // set granularity of layer material mapping and where material should be
    // mapped
    // hand over modules to ACTS
    Acts::ActsExtension::Config layConfig;
    layConfig.isLayer             = true;
    Acts::ActsExtension* detlayer = new Acts::ActsExtension(layConfig);
    lay_det.addExtension<Acts::IActsExtension>(detlayer);
    // Place layer volume
    PlacedVolume placedLayer = tube_vol.placeVolume(layer_vol);
    placedLayer.addPhysVolID("layer", layer_num);
    // Assign layer DetElement to layer volume
    lay_det.setPlacement(placedLayer);
    ++layer_num;
  }
  // Place Volume
  Volume       mother_vol = lcdd.pickMotherVolume(cylinderVolume);
  PlacedVolume placedTube = mother_vol.placeVolume(tube_vol);
  placedTube.addPhysVolID("system", cylinderVolume.id());
  cylinderVolume.setPlacement(placedTube);

  return cylinderVolume;
}

DECLARE_DETELEMENT(ACTS_LayerBarrel, create_element)
