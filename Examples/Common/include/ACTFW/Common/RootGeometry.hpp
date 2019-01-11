// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/RootDetector/BuildRootDetector.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "detail/VoidMaterialReader.hpp"

/// @brief adding some specific options for this geometry type
struct RootOptions
{
  /// @brief operator to be called to add options for the generic detector
  ///
  // @tparam options_t Type of the options object
  ///@param opt Options object
  template <typename options_t>
  void
  operator()(options_t& opt)
  {
    FW::Options::addRootGeometryOptions<options_t>(opt);
  }
};

/// @brief geometry getter, the operator() will be called int he example base
struct RootGeometry
{

  std::vector<std::shared_ptr<const Acts::TGeoDetectorElement>> detElementStore;

  /// @brief operator called to construct the tracking geometry
  ///
  /// @tparam variable_map_t Type of the variable map template for parameters
  /// @tparam material_reader_t the source for the surface material map
  ///
  /// @param vm the parameter map object
  ///
  /// @return a closed TrackingGeometry object
  template <typename variable_map_t,
            typename material_reader_t = VoidMaterialReader>
  std::shared_ptr<const Acts::TrackingGeometry>
  operator()(variable_map_t&          vm,
             const material_reader_t& mreader = VoidMaterialReader())
  {
    return FW::Root::buildRootDetector<variable_map_t>(vm, detElementStore);
  }
};