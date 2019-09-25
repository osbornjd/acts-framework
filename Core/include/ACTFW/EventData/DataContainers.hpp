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
#include <boost/container/flat_map.hpp>
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

/// Store elements that are identified by an index, e.g. in another container.
///
/// Each index can have zero or more associated elements. A typical case could
/// be to store all generating particles for a hit where the hit is identified
/// by its index in the hit container.
template <typename Value, typename Key = size_t>
using IndexMultimap = boost::container::flat_multimap<Key, Value>;

/// Invert the multimap, i.e. from a -> {b...} to b -> {a...}.
///
/// @note This assumes that the value in the initial multimap is itself a
///       sortable index-like object, as would be the case when mapping e.g.
///       hit ids to particle ids/ barcodes.
template <typename Value, typename Key>
inline IndexMultimap<Value, Key>
invertIndexMultimap(const IndexMultimap<Value, Key>& multimap)
{
  // switch key-value without enforcing the new ordering (linear copy)
  typename IndexMultimap<Key, Value>::sequence_type unordered;
  unordered.reserve(multimap.size());
  for (const auto& keyValue : multimap) {
    // value is now the key and the key is now the value
    unordered.emplace_back(keyValue.second, keyValue.first);
  }
  // adopting the unordered sequence will reestablish the correct order
  IndexMultimap<Key, Value> inverse;
  inverse.adopt_sequence(std::move(unordered));
  return inverse;
}

}  // namespace FW
