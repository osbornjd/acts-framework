// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef ACTFW_PLUGINS_HEPMCREADER_H
#define ACTFW_PLUGINS_HEPMCREADER_H

#include <fstream>
#include <iostream>
#include <mutex>
#include "HepMC/GenEvent.h"

namespace FW {

/// @class ObjSurfaceWriter
///
/// An Obj writer for the geometry
///
class HepMCReader
{
public:

  HepMC::GenEvent ge;
  
  void print() {ge.print();}
  void read(std::istream& is) {ge.read(is);};

  
};
} // FW
#endif  // ACTFW_PLUGINS_HEPMCREADER_H
