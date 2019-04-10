// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <iostream>
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
  FW::GeoModelReader gmr;
  GeoPhysVol*        world = gmr.loadDB("/home/user/geometry.db");

  // Walk over all children of the current volume
  unsigned int nChildren = world->getNChildVols();
  for (unsigned int i = 0; i < nChildren; i++) {
    PVConstLink nodeLink = world->getChildVol(i);
    // Test if it inherits from GeoVPhysVol
    if (dynamic_cast<const GeoVPhysVol*>(&(*(nodeLink)))) {
      const GeoVPhysVol* childVolV = &(*(nodeLink));

      if (childVolV->getLogVol()->getName() == "BeamPipeCentral") {
        std::shared_ptr<Acts::TrackingVolume> trVol
            = gmr.buildCentralBeamPipe(childVolV);

        auto& volBounds = trVol->volumeBounds();
        volBounds.dump(std::cout);
        auto layArray = trVol->confinedLayers();
        std::cout << "layArray: " << layArray << std::endl;
        auto layers = layArray->arrayObjects();
        std::cout << "numLayers: " << layers.size() << std::endl;
        for (auto l : layers) {
          std::cout << "thickness: " << l->thickness()
                    << "\tLayerType: " << l->layerType() << std::endl;
          auto surArray = l->surfaceArray();
          if (surArray) {
            surArray->dump(std::cout);
          }
        }
      } else if (childVolV->getLogVol()->getName() == "BeamPipeFwd") {
        gmr.buildFwdBeamPipe(childVolV);
      }
    }
  }

  //~ gmr.treeToStream(world, std::cout);
}
