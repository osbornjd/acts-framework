// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// RootInterpolatedBFieldWriter.hpp
///////////////////////////////////////////////////////////////////
#ifndef ACTFW_BFIELD_ROOTINTERPOLAREDBFIELDWRITER_H
#define ACTFW_BFIELD_ROOTINTERPOLAREDBFIELDWRITER_H

#include <TTree.h>
#include <mutex>
#include "ACTFW/Framework/IService.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"
#include "ACTS/Utilities/Logger.hpp"

namespace Acts {
class InterpolatedBFieldMap;
}

namespace FW {

namespace BField {

  /// @enum gridType
  /// Describes the axes definition of the grid of the magnetic field map
  enum GridType { rz = 0, xyz = 1 };

  /// @class RootInterpolatedBFieldWriter
  ///
  /// Writes out the Acts::InterpolatedbFieldMap. Currently implemented for 'rz'
  /// and 'xyz' field maps.

  class RootInterpolatedBFieldWriter
  {
  public:
    struct Config
    {
      /// The name of the output tree
      std::string treeName = "TTree";
      /// The name of the output file
      std::string fileName = "TFile.root";
      /// the file access mode (recreate by default)
      std::string fileMode = "recreate";
      /// The magnetic field to be written out
      std::shared_ptr<const Acts::InterpolatedBFieldMap> bField = nullptr;
      /// The axes definition of the grid of the magnetic field map
      GridType gridType = xyz;
    };

    /// Write down an interpolated magnetic field map
    static void
    run(const Config&                       cfg,
        std::unique_ptr<const Acts::Logger> logger
        = Acts::getDefaultLogger("RootInterpolatedBFieldWriter",
                                 Acts::Logging::INFO));
  };

}  // end of namespace BField

}  // end of namespace FW

#endif  // ACTFW_BFIELD_ROOTINTERPOLAREDBFIELDWRITER_H
