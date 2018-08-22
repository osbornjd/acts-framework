// This file is part of the Acts project.
//
// Copyright (C) 2016-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// DetectorElementBase.h, Acts project
///////////////////////////////////////////////////////////////////

#pragma once

#define ACTS_DETECTOR_ELEMENT_IDENTIFY_SPECIFIER override
#define ACTS_DETECTOR_ELEMENT_DIGIMODULE_SPECIFIER override

/// Set the identifier PLUGIN
#ifdef ACTS_CORE_IDENTIFIER_PLUGIN
#include ACTS_CORE_IDENTIFIER_PLUGIN
#else
typedef unsigned long long Identifier;
#endif

namespace Acts {
class DigitizationModule;
}

namespace FW {
namespace Detector {
  // @brief The detector element parent
  class DetectorElementParent
  {

  public:
    // Virtual destructor
    virtual ~DetectorElementParent() = default;

    /// Identifier
    virtual Identifier
    identifier() const = 0;

    /// Return the DigitizationModule
    /// @return optionally the DigitizationModule
    virtual std::shared_ptr<const Acts::DigitizationModule>
    digitizationModule() const = 0;
  };

}  // end of namespace Detector
}  // end of namespace FW

// now define the parent
#define ACTS_DETECTOR_ELEMENT_PARENT FW::Detector::DetectorElementParent
