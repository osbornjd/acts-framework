// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Json/JsonMaterialReader.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Acts/Material/ProtoSurfaceMaterial.hpp"
#include "Acts/Utilities/BinUtility.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Utilities/GeometryID.hpp"

FW::Json::JsonMaterialReader::JsonMaterialReader(
    const FW::Json::JsonMaterialReader::Config& cfg)
  : FW::IReaderT<Acts::SurfaceMaterialMap>(), m_cfg(cfg)
{
  // Validate the configuration
  if (m_cfg.fileName.empty()) {
    throw std::invalid_argument("Missing file name");
  } else if (!m_cfg.logger) {
    throw std::invalid_argument("Missing logger");
  }
}

FW::ProcessCode
FW::Json::JsonMaterialReader::read(
    Acts::SurfaceMaterialMap& sMaterialMap,
    size_t /*skip*/,
    const FW::AlgorithmContext* /*ctx*/)
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_read_mutex);
  
  ACTS_VERBOSE("Reading material from: " << m_cfg.fileName);

  std::ifstream ifj(m_cfg.fileName);
  json j;
  ifj >> j;
 
  ACTS_VERBOSE("Found entries for " << j.count(m_cfg.voltag) << " volume(s).") ;
  
  // structured binding 
  for (auto& [key, value] : j.items()) {
     // check if this the volume tag
     if (key == m_cfg.voltag){
         // get the volume json 
         auto volj = value;
         for (auto& [vkey, vvalue] : volj.items()){
             // create the volume id
             int vid = std::stoi(vkey);
             Acts::GeometryID volID(vid, Acts::GeometryID::volume_mask);
             ACTS_VERBOSE("-> Found Volume " << vid);
             // loop through the information in the volume
             for (auto& [vckey, vcvalue] : vvalue.items()){
                 if (vckey == m_cfg.boutag and not vcvalue.empty()){
                     ACTS_VERBOSE("--> BoundarySurface(s) to be parsed");
                 } else if (vckey == m_cfg.laytag){
                     ACTS_VERBOSE("--> Layer(s) to be parsed");
                     // now loop over layers
                     auto layj = vcvalue;
                     for (auto& [lkey, lvalue] : layj.items() ){
                         // create the layer id
                         int lid = std::stoi(lkey);
                         Acts::GeometryID layID(volID);
                         layID.add(lid, Acts::GeometryID::layer_mask);
                         ACTS_VERBOSE("---> Found Layer " << lid);
                         // finally loop over layer components
                         for (auto& [lckey, lcvalue] : lvalue.items()){
                             if (lckey == m_cfg.reptag and not lcvalue.empty()){
                               ACTS_VERBOSE("----> Found representing surface");
                               auto repmat = loadSurfaceMaterial(lcvalue);
                             } else if (lckey == m_cfg.apptag and not lcvalue.empty()) {
                               ACTS_VERBOSE("----> Found approach surface(s)");
                               // loop over approach surfaces
                               for (auto& [askey, asvalue] : lcvalue.items()){
                                 // create the layer id, todo set to max value
                                 int aid = (askey == "*") ? 0 : std::stoi(askey);
                                 Acts::GeometryID appID(layID);
                                 appID.add(aid, Acts::GeometryID::approach_mask);
                                 ACTS_VERBOSE("-----> Approach surface " << askey);
                                 auto appmat = loadSurfaceMaterial(asvalue);
                               }
                             } else if (lckey == m_cfg.sentag and not lcvalue.empty()) {
                               ACTS_VERBOSE("----> Found sensitive surface(s)");
                               // loop over sensitive surfaces
                               for (auto& [sskey, ssvalue] : lcvalue.items()){
                                 // create the layer id, todo set to max value
                                 int sid = (sskey == "*") ? 0 : std::stoi(sskey);
                                 Acts::GeometryID senID(layID);
                                 senID.add(sid, Acts::GeometryID::sensitive_mask);
                                 ACTS_VERBOSE("-----> Sensitive surface " << sskey);
                                 auto senmat = loadSurfaceMaterial(ssvalue);
                               }
                             }
                         }
                     }
                     
                 } else if (vckey == m_cfg.mattag and not vcvalue.empty()){
                     ACTS_VERBOSE("--> VolumeMaterial to be parsed");
                 }
             } 
         }
     } else if ( key == m_cfg.geoversion){
         ACTS_VERBOSE("Detector version: " << m_cfg.geoversion);
     }
  }  
  
  // Announce success
  return FW::ProcessCode::SUCCESS;
}


/// Create the Surface Material
std::shared_ptr<const Acts::ISurfaceMaterial> 
FW::Json::JsonMaterialReader::loadSurfaceMaterial(json material)
{
  
  return nullptr;
}


