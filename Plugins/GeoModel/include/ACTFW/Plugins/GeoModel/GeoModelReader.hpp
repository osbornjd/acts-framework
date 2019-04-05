// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "Acts/Surfaces/Surface.hpp"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

#include <QString>

namespace FW {

  class GeoModelReader
  {
  public:
	GeoPhysVol* 
	createTheExperiment(GeoPhysVol* world) const;

GeoPhysVol* 
loadDB(const QString& path) const;
  
    GeoPhysVol* 
    makeDetektor() const;
    
    std::vector<std::shared_ptr<Acts::Surface>>
    buildBeamPipeSurfaces(GeoVPhysVol const* bp) const;
    
    /// @brief Printer of the full detector
    std::ostream&
    treeToStream(GeoVPhysVol const* tree, std::ostream& sl) const;
    
    /// @brief Printer of GeoPhysVol
    std::ostream&
  toStream(GeoPhysVol const* gpv, std::ostream& sl) const;
    
    /// @brief Printer of GeoFullPhysVol
    std::ostream&
  toStream(GeoFullPhysVol const* gfpv, std::ostream& sl) const;
    
    private:
    
std::shared_ptr<Acts::Surface>
createSurface(GeoVPhysVol const* gvpv) const;

    /// @brief Printer of GeoLogVol
    std::ostream&
    toStream(GeoLogVol const* glv, std::ostream& sl) const;
    
    std::ostream&
    shapeToStream(GeoShape const* shape, std::ostream& sl) const;
    
   };
}  // namespace FW
