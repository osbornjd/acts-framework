// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include <Acts/Geometry/GeometryID.hpp>
#include <boost/container/flat_set.hpp>

#include "ACTFW/Utilities/Range.hpp"

namespace FW {
namespace detail {
  struct CompareGeometryId
  {
    // indicate allowed transparent comparisons between ids and full objects
    using is_transparent = void;
    // support direct comparision with geometry ids
    inline geo_id_value
    key(Acts::GeometryID geoId) const
    {
      return geoId.value();
    }
    // support comparison for items in map-like structures
    template <typename T>
    inline geo_id_value
    key(const std::pair<Acts::GeometryID, T>& mapItem) const
    {
      return mapItem.first.value();
    }
    // support comparison for items that implement `.geoId()` directly
    template <typename T>
    inline auto
    key(const T& thing) const -> decltype(thing.geoId(), geo_id_value())
    {
      return thing.geoId().value();
    }
    // compare two elements using the automatic key extraction defined above
    template <typename Left, typename Right>
    inline bool
    operator()(const Left& left, const Right& right) const
    {
      return key(left) < key(right);
    }
  };

  /// Get the last geometry id below the mask and keep everything above.
  ///
  /// E.g 0x00101000 with mask 0x00111100 would become 0x00101011.
  inline Acts::GeometryID
  getLastId(Acts::GeometryID value, geo_id_value mask)
  {
    // ctzll = count trailing zeros for long long
    geo_id_value firstNonZero = (1u << __builtin_ctzll(mask));
    // a value with all trailing bits (according to the mask) set to one
    geo_id_value trailingOnes = firstNonZero - 1;
    return value.value() | trailingOnes;
  }
}  // namespace detail

/// Store elements that know their detector geometry id, e.g. simulation hits.
///
/// @tparam T type to be stored, must be compatible with `CompareGeometryId`
///
/// The container stores an arbitrary number of elements for any geometry
/// id. Elements can be retrieved via the geometry id; elements can be selected
/// for a specific geometry id or for a larger range, e.g. a volume or a layer
/// within the geometry hierachy using the helper functions below. Elements can
/// also be accessed by index that uniquely identifies each element regardless
/// of geometry id.
template <typename T>
using GeometryIdMultiset
    = boost::container::flat_multiset<T, detail::CompareGeometryId>;

/// Store elements indexed by an geometry id.
///
/// @tparam T type to be stored
///
/// The behaviour is the same as for the `GeometryIdMultiset` except that the
/// stored elements do not know their geometry id themself. When iterating
/// the iterator elements behave as for the `std::map`, i.e.
///
///     for (const auto& entry: elements) {
///         auto id = entry.first; // geometry id
///         const auto& el = entry.second; // stored element
///     }
///
template <typename T>
using GeometryIdMultimap = GeometryIdMultiset<std::pair<Acts::GeometryID, T>>;

/// Select all elements within the given volume.
template <typename T>
inline Range<typename GeometryIdMultiset<T>::const_iterator>
selectVolume(const GeometryIdMultiset<T>& container, geo_id_value volumne)
{
  Acts::GeometryID first(volumne, Acts::GeometryID::volume_mask);
  Acts::GeometryID last(
      detail::getLastId(first, Acts::GeometryID::volume_mask));
  return makeRange(container.lower_bound(first), container.upper_bound(last));
}

/// Select all elements within the given layer.
template <typename T>
inline Range<typename GeometryIdMultiset<T>::const_iterator>
selectLayer(const GeometryIdMultiset<T>& container,
            geo_id_value                 volumne,
            geo_id_value                 layer)
{
  Acts::GeometryID first(volumne, Acts::GeometryID::volume_mask);
  first.add(layer, Acts::GeometryID::layer_mask);
  Acts::GeometryID last(detail::getLastId(first, Acts::GeometryID::layer_mask));
  return makeRange(container.lower_bound(first), container.upper_bound(last));
}

/// Select all elements for the given module / sensitive surface.
template <typename T>
inline Range<typename GeometryIdMultiset<T>::const_iterator>
selectModule(const GeometryIdMultiset<T>& container,
             geo_id_value                 volumne,
             geo_id_value                 layer,
             geo_id_value                 module)
{
  // module is the lowest level and defines a single geometry id value
  Acts::GeometryID geoId(volumne, Acts::GeometryID::volume_mask);
  geoId.add(layer, Acts::GeometryID::layer_mask);
  geoId.add(module, Acts::GeometryID::sensitive_mask);
  return makeRange(container.equal_range(geoId));
}

/// Data containers designed to fit around the GeometryID structure
///
/// internal map structure is
/// { volume : layer : module , data }
template <typename data_t>
using ModuleData = std::vector<data_t>;
template <typename identifier_t, typename data_t>
using LayerData = std::map<identifier_t, ModuleData<data_t>>;
template <typename identifier_t, typename data_t>
using VolumeData = std::map<identifier_t, LayerData<identifier_t, data_t>>;
template <typename identifier_t, typename data_t>
using DetectorData = std::map<identifier_t, VolumeData<identifier_t, data_t>>;

namespace Data {

  /// @brief Insert (& create container if necessary)
  ///
  /// @tparam identifier_t Type of the identifier key
  /// @tparam data_t Type of the object to be filled in
  ///
  /// @param dData The detector data map
  /// @param volumeKey The identifier for the detector volume
  /// @param layerKey The identifier for the layer
  /// @param moduleKey The identifier for the module
  /// @param obj The data object to be stored
  template <typename identifier_t, typename data_t>
  void
  insert(DetectorData<identifier_t, data_t>& dData,
         identifier_t                        volumeKey,
         identifier_t                        layerKey,
         identifier_t                        moduleKey,
         data_t                              obj)
  {
    // find if the volume has an entry
    auto volumeData = dData.find(volumeKey);
    if (volumeData == dData.end()) {
      // insert at the volumeKey
      dData[volumeKey] = FW::VolumeData<identifier_t, data_t>();
      volumeData       = dData.find(volumeKey);
    }
    // find the layer data
    auto layerData = (volumeData->second).find(layerKey);
    if (layerData == (volumeData->second).end()) {
      // insert a layer key for this
      (volumeData->second)[layerKey] = FW::LayerData<identifier_t, data_t>();
      layerData                      = (volumeData->second).find(layerKey);
    }
    // find the module data
    auto moduleData = (layerData->second).find(moduleKey);
    if (moduleData == (layerData->second).end()) {
      // insert the module for this
      (layerData->second)[moduleKey] = FW::ModuleData<data_t>();
      moduleData                     = (layerData->second).find(moduleKey);
    }
    // and now push back
    (moduleData->second).push_back(std::move(obj));
  };

  /// @brief Read and return the module data
  ///
  /// @tparam identifier_t Type of the identifier key
  /// @tparam data_t Type of the object to be filled in
  ///
  /// @param dData The detector data map
  /// @param volumeKey The identifier for the detector volume
  /// @param layerKey The identifier for the layer
  /// @param moduleKey The identifier for the module
  template <typename identifier_t, typename data_t>
  const ModuleData<identifier_t>*
  read(DetectorData<identifier_t, data_t>& dData,
       identifier_t                        volumeKey,
       identifier_t                        layerKey,
       identifier_t                        moduleKey)
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

  /// @brief Read and return the layer data
  ///
  /// @tparam identifier_t Type of the identifier key
  /// @tparam data_t Type of the object to be filled in
  ///
  /// @param dData The detector data map
  /// @param volumeKey The identifier for the detector volume
  /// @param layerKey The identifier for the layer
  /// @param moduleKey The identifier for the module
  template <typename identifier_t, typename data_t>
  const LayerData<identifier_t, data_t>*
  read(DetectorData<identifier_t, data_t>& dData,
       identifier_t                        volumeKey,
       identifier_t                        layerKey)
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
}  // namespace FW
