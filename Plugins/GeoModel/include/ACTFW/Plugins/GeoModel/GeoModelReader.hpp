// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iostream>
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoFullPhysVol.h"

namespace FW {

  class GeoModelReader
  {
  public:
    GeoPhysVol* makeDetektor();
    
    std::ostream&
  toStream(GeoPhysVol const* gpv, std::ostream& sl) const;
    
    std::ostream&
  toStream(GeoFullPhysVol const* gfpv, std::ostream& sl) const;
    
    private:
    std::ostream&
    toStream(GeoLogVol const* glv, std::ostream& sl) const;
   };
}  // namespace FW
