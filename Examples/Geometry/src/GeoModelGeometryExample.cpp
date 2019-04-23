// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <QString>
#include <iostream>
#include <string>
#include "ACTFW/Plugins/GeoModel/GeoModelBeamPipe.hpp"
#include "ACTFW/Plugins/GeoModel/GeoModelReader.hpp"
#include "Acts/Detector/TrackingVolume.hpp"
#include "Acts/Volumes/VolumeBounds.hpp"
#include "GeoModelKernel/GeoFullPhysVol.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoPVLink.h"
#include "GeoModelKernel/GeoPhysVol.h"

/// @brief main executable
///
/// @param argc The argument count
/// @param argv The argument list
int
main(int argc, char* argv[])
{
  // Exit if no file is given
  if (argc < 2) {
    std::cout << "No file path provided - exiting." << std::endl;
    return 0;
  }
  FW::GeoModelReader gmr;
  QString            path(argv[1]);
  GeoPhysVol*        world = gmr.loadDB(path);

  FW::GeoModelBeamPipe gmbp;

  // Walk over all children of the current volume
  unsigned int nChildren = world->getNChildVols();
  for (unsigned int i = 0; i < nChildren; i++) {
    PVConstLink nodeLink = world->getChildVol(i);
    // Test if it inherits from GeoVPhysVol
    if (dynamic_cast<const GeoVPhysVol*>(&(*(nodeLink)))) {
      const GeoVPhysVol* childVolV = &(*(nodeLink));

      std::vector<std::shared_ptr<Acts::TrackingVolume>> trVols;

      // Build the object depending on its name
      if (childVolV->getLogVol()->getName() == "BeamPipeCentral") {
        trVols.push_back(gmbp.buildBeamPipe(childVolV));
      } else if (childVolV->getLogVol()->getName() == "BeamPipeFwd") {
        trVols.push_back(gmbp.buildBeamPipe(childVolV));
      }
    }
  }
}