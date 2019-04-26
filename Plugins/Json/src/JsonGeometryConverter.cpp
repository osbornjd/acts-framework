// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Json/JsonGeometryConverter.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Material/HomogeneousSurfaceMaterial.hpp"
#include "Acts/Material/ProtoSurfaceMaterial.hpp"
#include "Acts/Utilities/BinUtility.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Utilities/GeometryID.hpp"

FW::Json::JsonGeometryConverter::JsonGeometryConverter(
    const FW::Json::JsonGeometryConverter::Config& cfg)
  : m_cfg(cfg)
{
  // Validate the configuration
  if (!m_cfg.logger) {
    throw std::invalid_argument("Missing logger");
  }
}

std::pair<Acts::SurfaceMaterialMap, Acts::VolumeMaterialMap>
FW::Json::JsonGeometryConverter::jsonToMaterialMaps(const json& materialmaps)
{

  auto& j = materialmaps;
  // The return maps
  std::pair<Acts::SurfaceMaterialMap, Acts::VolumeMaterialMap> maps;
  ACTS_VERBOSE("j2a: Reading material maps from json file.");
  ACTS_VERBOSE("j2a: Found entries for " << j.count(m_cfg.volkey)
                                         << " volume(s).");

  // structured binding
  for (auto & [ key, value ] : j.items()) {
    // check if this the volume key
    if (key == m_cfg.volkey) {
      // get the volume json
      auto volj = value;
      for (auto & [ vkey, vvalue ] : volj.items()) {
        // create the volume id
        int              vid = std::stoi(vkey);
        Acts::GeometryID volID(vid, Acts::GeometryID::volume_mask);
        ACTS_VERBOSE("j2a: -> Found Volume " << vid);
        // loop through the information in the volume
        for (auto & [ vckey, vcvalue ] : vvalue.items()) {
          if (vckey == m_cfg.boukey and not vcvalue.empty()) {
            ACTS_VERBOSE("j2a: --> BoundarySurface(s) to be parsed");
          } else if (vckey == m_cfg.laykey) {
            ACTS_VERBOSE("j2a: --> Layer(s) to be parsed");
            // now loop over layers
            auto layj = vcvalue;
            for (auto & [ lkey, lvalue ] : layj.items()) {
              // create the layer id
              int              lid = std::stoi(lkey);
              Acts::GeometryID layID(volID);
              layID.add(lid, Acts::GeometryID::layer_mask);
              ACTS_VERBOSE("j2a: ---> Found Layer " << lid);
              // finally loop over layer components
              for (auto & [ lckey, lcvalue ] : lvalue.items()) {
                if (lckey == m_cfg.repkey and not lcvalue.empty()) {
                  ACTS_VERBOSE("j2a: ----> Found representing surface");
                  auto repmat       = jsonToSurfaceMaterial(lcvalue);
                  maps.first[layID] = repmat;
                } else if (lckey == m_cfg.appkey and not lcvalue.empty()) {
                  ACTS_VERBOSE("j2a: ----> Found approach surface(s)");
                  // loop over approach surfaces
                  for (auto & [ askey, asvalue ] : lcvalue.items()) {
                    // create the layer id, todo set to max value
                    int aid = (askey == "*") ? 0 : std::stoi(askey);
                    Acts::GeometryID appID(layID);
                    appID.add(aid, Acts::GeometryID::approach_mask);
                    ACTS_VERBOSE("j2a: -----> Approach surface " << askey);
                    auto appmat       = jsonToSurfaceMaterial(asvalue);
                    maps.first[appID] = appmat;
                  }
                } else if (lckey == m_cfg.senkey and not lcvalue.empty()) {
                  ACTS_VERBOSE("j2a: ----> Found sensitive surface(s)");
                  // loop over sensitive surfaces
                  for (auto & [ sskey, ssvalue ] : lcvalue.items()) {
                    // create the layer id, todo set to max value
                    int sid = (sskey == "*") ? 0 : std::stoi(sskey);
                    Acts::GeometryID senID(layID);
                    senID.add(sid, Acts::GeometryID::sensitive_mask);
                    ACTS_VERBOSE("j2a: -----> Sensitive surface " << sskey);
                    auto senmat       = jsonToSurfaceMaterial(ssvalue);
                    maps.first[senID] = senmat;
                  }
                }
              }
            }

          } else if (vckey == m_cfg.matkey and not vcvalue.empty()) {
            ACTS_VERBOSE("--> VolumeMaterial to be parsed");
          }
        }
      }
    } else if (key == m_cfg.geoversion) {
      ACTS_VERBOSE("Detector version: " << m_cfg.geoversion);
    }
  }

  // Return the filled maps
  return maps;
}

/// Convert method
///
json
FW::Json::JsonGeometryConverter::materialMapsToJson(
    std::pair<Acts::SurfaceMaterialMap, Acts::VolumeMaterialMap>& maps)
{
  DetectorRep detRep;
  // Collect all GeometryIDs per VolumeID for the formatted output
  for (auto & [ key, value ] : maps.first) {
    geo_id_value vid    = key.value(Acts::GeometryID::volume_mask);
    auto         volRep = detRep.volumes.find(vid);
    if (volRep == detRep.volumes.end()) {
      detRep.volumes.insert({vid, VolumeRep()});
      volRep = detRep.volumes.find(vid);
    }
    geo_id_value lid = key.value(Acts::GeometryID::layer_mask);
    if (lid != 0) {
      // we are on a layer, get the layer rep
      auto layRep = volRep->second.layers.find(lid);
      if (layRep == volRep->second.layers.end()) {
        volRep->second.layers.insert({lid, LayerRep()});
        layRep = volRep->second.layers.find(lid);
      }
      // now insert appropriately
      geo_id_value sid = key.value(Acts::GeometryID::sensitive_mask);
      geo_id_value aid = key.value(Acts::GeometryID::approach_mask);
      if (sid != 0) {
        layRep->second.sensitives.insert({key, value});
      } else if (aid != 0) {
        layRep->second.approaches.insert({key, value});
      } else {
        layRep->second.representing = value;
      }

    } else {
      // not on a layer can only be a boundary surface
      volRep->second.boundaries.insert({key, value});
    }
  }
  for (auto & [ key, value ] : maps.second) {
    // find the volume representation
    geo_id_value vid    = key.value(Acts::GeometryID::volume_mask);
    auto         volRep = detRep.volumes.find(vid);
    if (volRep == detRep.volumes.end()) {
      detRep.volumes.insert({vid, VolumeRep()});
      volRep = detRep.volumes.find(vid);
    }
    volRep->second.material = value;
  }
  // convert the detector representation to json format
  return detectorRepToJson(detRep);
}

/// Create Json from a detector represenation
json
FW::Json::JsonGeometryConverter::detectorRepToJson(const DetectorRep& detRep)
{
  json detectorj;
  ACTS_VERBOSE("a2j: Writing json from detector representation");
  ACTS_VERBOSE("a2j: Found entries for " << detRep.volumes.size()
                                         << " volume(s).");

  json volumesj;
  for (auto & [ key, value ] : detRep.volumes) {
    json volj;
    ACTS_VERBOSE("a2j: -> Writing Volume: " << key);
    // write the layers
    if (not value.layers.empty()) {
      ACTS_VERBOSE("a2j: ---> Found " << value.layers.size() << " layer(s) ");
      json layersj;
      for (auto & [ lkey, lvalue ] : value.layers) {
        ACTS_VERBOSE("a2j: ----> Convert layer " << lkey);
        json layj;
        // first check for approaches
        if (not lvalue.approaches.empty()) {
          ACTS_VERBOSE("a2j: -----> Found " << lvalue.approaches.size()
                                            << " approach surface(s)");
          json approachesj;
          for (auto & [ akey, avalue ] : lvalue.approaches) {
            geo_id_value aid = akey.value(Acts::GeometryID::approach_mask);
            ACTS_VERBOSE("a2j: ------> Convert approach surface " << aid);
            approachesj[std::to_string(aid)]
                = surfaceMaterialToJson(*avalue.get());
          }
          // add to the layer json
          layj[m_cfg.appkey] = approachesj;
        }
        // then check for sensitive
        if (not lvalue.sensitives.empty()) {
          ACTS_VERBOSE("a2j: -----> Found " << lvalue.sensitives.size()
                                            << " sensitive surface(s)");
          json sensitivesj;
          for (auto & [ skey, svalue ] : lvalue.sensitives) {
            geo_id_value sid = skey.value(Acts::GeometryID::sensitive_mask);
            ACTS_VERBOSE("a2j: ------> Convert sensitive surface " << sid);
            sensitivesj[std::to_string(sid)]
                = surfaceMaterialToJson(*svalue.get());
          }
          // add to the layer json
          layj[m_cfg.senkey] = sensitivesj;
        }
        // finally check for representing
        if (lvalue.representing != nullptr) {
          ACTS_VERBOSE("a2j: ------> Convert representing surface ");
          layj[m_cfg.repkey]
              = surfaceMaterialToJson(*lvalue.representing.get());
        }
        layersj[std::to_string(lkey)] = layj;
      }
      volj[m_cfg.laykey] = layersj;
    }

    volumesj[std::to_string(key)] = volj;
  }

  detectorj[m_cfg.volkey] = volumesj;

  return detectorj;
}

/// Create the Surface Material
std::shared_ptr<const Acts::ISurfaceMaterial>
FW::Json::JsonGeometryConverter::jsonToSurfaceMaterial(const json& material)
{

  std::shared_ptr<const Acts::ISurfaceMaterial> sMaterial = nullptr;
  // The bin utility for deescribing the data
  Acts::BinUtility bUtility;
  // Convert the material
  Acts::MaterialPropertiesMatrix mpMatrix;
  // Structured binding
  for (auto & [ key, value ] : material.items()) {
    // Check json keys
    if (key == m_cfg.bin0key and not value.empty()) {
      bUtility += jsonToBinUtility(value);
    } else if (key == m_cfg.bin1key and not value.empty()) {
      bUtility += jsonToBinUtility(value);
    }
    if (key == m_cfg.datakey and not value.empty()) {
      mpMatrix = jsonToMaterialMatrix(value);
    }
  }

  // We have protoMaterial
  if (mpMatrix.size() == 0) {
    sMaterial = std::make_shared<const Acts::ProtoSurfaceMaterial>(bUtility);
  } else if (bUtility.bins() == 1) {
    sMaterial = std::make_shared<const Acts::HomogeneousSurfaceMaterial>(
        mpMatrix[0][0]);
  } else {
    sMaterial = std::make_shared<const Acts::BinnedSurfaceMaterial>(bUtility,
                                                                    mpMatrix);
  }
  // return what you have
  return sMaterial;
}

json
FW::Json::JsonGeometryConverter::surfaceMaterialToJson(
    const Acts::ISurfaceMaterial& sMaterial)
{
  json smj;
  // if a bin utility is to be written
  const Acts::BinUtility* bUtility = nullptr;
  // check if we have a proto material
  auto psMaterial = dynamic_cast<const Acts::ProtoSurfaceMaterial*>(&sMaterial);
  if (psMaterial != nullptr) {
    // type is proto
    smj[m_cfg.typekey] = "proto";
    bUtility           = &(psMaterial->binUtility());
  } else {
    // now check if we have a homogeneous material
    auto hsMaterial
        = dynamic_cast<const Acts::HomogeneousSurfaceMaterial*>(&sMaterial);
    if (hsMaterial != nullptr) {
      // type is homogeneous
      smj[m_cfg.typekey] = "homogeneous";
      // smj[m_cfg.datakey] = convert the data
    } else {
      // only option remaining: BinnedSurface material
      // now check if we have a homogeneous material
      auto bsMaterial
          = dynamic_cast<const Acts::BinnedSurfaceMaterial*>(&sMaterial);
      if (bsMaterial != nullptr) {
        // type is binned
        smj[m_cfg.typekey] = "binned";
        bUtility           = &(psMaterial->binUtility());
        // convert the data
        // smj[m_cfg.datakey] =
      }
    }
  }
  // add the bin utility
  if (bUtility != nullptr) {
    std::vector<std::string> binkeys = {m_cfg.bin0key, m_cfg.bin1key};
    // loop over dimensions and write
    auto& binningData = bUtility->binningData();
    // loop over the dimensions
    for (size_t ibin = 0; ibin < binningData.size(); ++ibin) {
      json binj;
      auto cbData = binningData[ibin];
      binj.push_back(Acts::binningValueNames[cbData.binvalue]);
      if (cbData.option == Acts::closed) {
        binj.push_back("closed");
      } else {
        binj.push_back("open");
      }
      binj.push_back(cbData.bins());
      smj[binkeys[ibin]] = binj;
    }
  }

  return smj;
}

/// Create the Material Matrix
Acts::MaterialPropertiesMatrix
FW::Json::JsonGeometryConverter::jsonToMaterialMatrix(const json& data)
{
  Acts::MaterialPropertiesMatrix mpMatrix;
  /// This is assumed to be an array or array of array[6]
  for (auto& outer : data) {
    Acts::MaterialPropertiesVector mpVector;
    for (auto& inner : outer) {
      if (inner.size() > 5) {
        mpVector.push_back(Acts::MaterialProperties(
            inner[0], inner[1], inner[2], inner[3], inner[4], inner[5]));
      } else {
        mpVector.push_back(Acts::MaterialProperties());
      }
    }
    mpMatrix.push_back(std::move(mpVector));
  }
  return mpMatrix;
}

/// Create the BinUtility for this
Acts::BinUtility
FW::Json::JsonGeometryConverter::jsonToBinUtility(const json& bin)
{
  // finding the iterator position to determine the binning value
  auto bit = std::find(
      Acts::binningValueNames.begin(), Acts::binningValueNames.end(), bin[0]);
  size_t             indx = std::distance(Acts::binningValueNames.begin(), bit);
  Acts::BinningValue bval = Acts::BinningValue(indx);
  Acts::BinningOption bopt = bin[1] == "open" ? Acts::open : Acts::closed;
  unsigned int        bins = bin[2];
  double              min, max = 0;
  if (bin[3].size() == 2) {
    min = bin[3][0];
    max = bin[3][1];
  }
  return Acts::BinUtility(bins, min, max, bopt, bval);
}
