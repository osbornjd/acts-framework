// This file is part of the Acts project.
//
// Copyright (C) 2017-2020 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>
#include <cstddef>
#include <utility>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include "ACTFW/Utilities/Range.hpp"
#include "Acts/Geometry/GeometryID.hpp"

namespace FW {
namespace detail {
  struct CompareGeometryId
  {
    // indicate allowed transparent comparisons between ids and full objects
    using is_transparent = void;
    // support direct comparision with geometry ids
    constexpr Acts::GeometryID
    key(Acts::GeometryID geoId) const
    {
      return geoId;
    }
    // support direct comparision with encoded geometry ids
    constexpr Acts::GeometryID
    key(Acts::GeometryID::Value encoded) const
    {
      return Acts::GeometryID(encoded);
    }
    // support comparison for items in map-like structures
    template <typename T>
    constexpr Acts::GeometryID
    key(const std::pair<Acts::GeometryID, T>& mapItem) const
    {
      return mapItem.first;
    }
    // support comparison for items that implement `.geometryId()` directly
    template <typename T>
    inline auto
    key(const T& thing) const
        -> decltype(thing.geometryId(), Acts::GeometryID())
    {
      return thing.geometryId();
    }
    // compare two elements using the automatic key extraction defined above
    template <typename Left, typename Right>
    constexpr bool
    operator()(Left&& left, Right&& right) const
    {
      return key(left) < key(right);
    }
  };
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
selectVolume(const GeometryIdMultiset<T>& container,
             Acts::GeometryID::Value      volume)
{
  auto cmp = Acts::GeometryID().setVolume(volume);
  auto beg = std::lower_bound(
      container.begin(), container.end(), cmp, detail::CompareGeometryId{});
  // WARNING overflows to volume==0 if the input volume is the last one
  cmp = Acts::GeometryID().setVolume(volume + 1u);
  // optimize search by using the lower bound as start point. also handles
  // volume overflows since the geo id would be located before the start of
  // the upper edge search window.
  auto end = std::lower_bound(
      beg, container.end(), cmp, detail::CompareGeometryId{});
  return makeRange(beg, end);
}
template <typename T>
inline auto
selectVolume(const GeometryIdMultiset<T>& container, Acts::GeometryID id)
{
  return selectVolume(container, id.volume());
}

/// Select all elements within the given layer.
template <typename T>
inline Range<typename GeometryIdMultiset<T>::const_iterator>
selectLayer(const GeometryIdMultiset<T>& container,
            Acts::GeometryID::Value      volume,
            Acts::GeometryID::Value      layer)
{
  auto cmp = Acts::GeometryID().setVolume(volume).setLayer(layer);
  auto beg = std::lower_bound(
      container.begin(), container.end(), cmp, detail::CompareGeometryId{});
  // WARNING resets to layer==0 if the input layer is the last one
  cmp = Acts::GeometryID().setVolume(volume).setLayer(layer + 1u);
  // optimize search by using the lower bound as start point. also handles
  // volume overflows since the geo id would be located before the start of
  // the upper edge search window.
  auto end = std::lower_bound(
      beg, container.end(), cmp, detail::CompareGeometryId{});
  return makeRange(beg, end);
}
template <typename T>
inline auto
selectLayer(const GeometryIdMultiset<T>& container, Acts::GeometryID id)
{
  return selectLayer(container, id.volume(), id.layer());
}

/// Select all elements for the given module / sensitive surface.
template <typename T>
inline Range<typename GeometryIdMultiset<T>::const_iterator>
selectModule(const GeometryIdMultiset<T>& container, Acts::GeometryID geoId)
{
  // module is the lowest level and defines a single geometry id value
  return makeRange(container.equal_range(geoId));
}
template <typename T>
inline auto
selectModule(const GeometryIdMultiset<T>& container,
             Acts::GeometryID::Value      volume,
             Acts::GeometryID::Value      layer,
             Acts::GeometryID::Value      module)
{
  return selectModule(
      container,
      Acts::GeometryID().setVolume(volume).setLayer(layer).setSensitive(
          module));
}

}  // namespace FW
