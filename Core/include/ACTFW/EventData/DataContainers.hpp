// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//
//  DataContainers.hpp
//  ACTFW
//
//  Created by Andreas Salzburger on 17/05/16.
//
//

#ifndef ACTFW_EVENTDATA_DATACONTAINERS_H
#define ACTFW_EVENTDATA_DATACONTAINERS_H

#include <map>
#include <vector>

namespace FW {

/// Data containers designed to fit around the GeometryID structure
///
/// internal map structure is
/// { volume : layer : module , data }
template <class T>
using ModuleData = std::vector<T>;
template <class U, class T>
using LayerData = std::map<U, ModuleData<T>>;
template <class U, class T>
using VolumeData = std::map<U, LayerData<U, T>>;
template <class U, class T>
using DetectorData = std::map<U, VolumeData<U, T>>;

namespace Data {

  // insert (& create container if necessary)
  template <class U, class T>
  void
  insert(DetectorData<U, T>& dData, U volumeKey, U layerKey, U moduleKey, T obj)
  {
    // find if the volume has an entry
    auto volumeData = dData.find(volumeKey);
    if (volumeData == dData.end()) {
      // insert at the volumeKey
      dData[volumeKey] = FW::VolumeData<U, T>();
      volumeData       = dData.find(volumeKey);
    }
    // find the layer data
    auto layerData = (volumeData->second).find(layerKey);
    if (layerData == (volumeData->second).end()) {
      // insert a layer key for this
      (volumeData->second)[layerKey] = FW::LayerData<U, T>();
      layerData                      = (volumeData->second).find(layerKey);
    }
    // find the module data
    auto moduleData = (layerData->second).find(moduleKey);
    if (moduleData == (layerData->second).end()) {
      // insert the module for this
      (layerData->second)[moduleKey] = FW::ModuleData<T>();
      moduleData                     = (layerData->second).find(moduleKey);
    }
    // and now push back
    (moduleData->second).push_back(std::move(obj));
  };

  // read (& return)
  template <class U, class T>
  const ModuleData<T>*
  read(DetectorData<U, T>& dData, U volumeKey, U layerKey, U moduleKey)
  {
    // find if the volume has an entry
    auto volumeData = dData.find(volumeKey);
    if (volumeData == dData.end()) return nullptr;
    // find the layer data
    auto layerData = (volumeData->second).find(layerKey);
    if (layerData == (volumeData->second).end()) return nullptr;
    // find the module data
    auto moduleData = (layerData->second).find(moduleKey);
    if (moduleData == (layerData->second).end()) return nullptr;
    // and now return as a pointer
    return (&(moduleData->second));
  };

  // read (& return)
  template <class U, class T>
  const LayerData<U, T>*
  read(DetectorData<U, T>& dData, U volumeKey, U layerKey)
  {
    // find if the volume has an entry
    auto volumeData = dData.find(volumeKey);
    if (volumeData == dData.end()) return nullptr;
    // find the layer data
    auto layerData = (volumeData->second).find(layerKey);
    if (layerData == (volumeData->second).end()) return nullptr;
    // and now return as a pointer
    return (&(layerData->second));
  };

}  // namespace Data
}

#endif  // ACTFW_EVENTDATA_DATACONTAINERS_H
