// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include "ACTFW/Plugins/DD4hep/DD4hepDetectorOptions.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"

/// @brief geometry getter, the operator() will be called int he example base
struct DD4hepGeometryGetter {
  
template <typename options_t>
std::shared_ptr<const Acts::TrackingGeometry> 
operator()(options_t& vm){
    // read the detector config & dd4hep detector
    auto dd4HepDetectorConfig
        = FW::Options::readDD4hepConfig<po::variables_map>(vm);
    auto geometrySvc
        = std::make_shared<FW::DD4hep::GeometryService>(dd4HepDetectorConfig);
    std::shared_ptr<const Acts::TrackingGeometry> dd4tGeometry
        = geometrySvc->trackingGeometry();
    return dd4tGeometry;
  } 
};

int
main(int argc, char* argv[])
{
  // --------------------------------------------------------------------------------
  DD4hepGeometryGetter tGeoGetter;
  // now process it
  return processGeometry(argc, argv, tGeoGetter);  
}
