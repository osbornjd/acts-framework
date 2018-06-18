// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "GeometryExampleBase.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"
#include "Acts/Utilities/Logger.hpp"
#include "ACTFW/RootDetector/BuildRootDetector.hpp"

/// @brief adding some specific options for this geometry type
struct RootOptions {

template <typename options_t>
void operator()(options_t& opt)
{
  FW::Options::addRootGeometryOptions<options_t>(opt);
}

};


/// @brief geometry getter, the operator() will be called int he example base
struct RootGeometry {
  
template <typename variable_map_t>
std::shared_ptr<const Acts::TrackingGeometry> 
operator()(variable_map_t& vm){  
  return FW::Root::buildRootDetector<variable_map_t>(vm);
} 
};


int
main(int argc, char* argv[])
{
  // --------------------------------------------------------------------------------
  RootOptions  rootOptions;
  RootGeometry rootGeometry;
  
  // now process it
  return processGeometry(argc, argv, rootOptions, rootGeometry);
}