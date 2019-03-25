// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <map>
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"
#include "Acts/Plugins/Identification/IdentifiedDetectorElement.hpp"
#include "Acts/Plugins/Identification/Identifier.hpp"
#include "Acts/Surfaces/Surface.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/GeometryContext.hpp"
#include "Acts/Utilities/GeometryID.hpp"

namespace FW {

namespace Alignable {

  /// @class AlignableGeoContext
  struct AlignableGeoContext
  {

    // The alignment store of this event
    // not the fastest, but good enough for a demonstrator
    std::vector<Acts::Transform3D> alignmentStore;
  };

  /// @class AlignableDetectorElement extends GenericDetectorElement
  ///
  /// This is a lightweight type of detector element,
  /// it simply implements the base class.
  ///
  class AlignableDetectorElement : public Generic::GenericDetectorElement
  {
  public:
    /// Constructor for an alignable surface
    ///
    /// @note see Generic::GenericDetectorElement for documentation
    template <typename... Args>
    AlignableDetectorElement(Args&&... args)
      : Generic::GenericDetectorElement(std::forward<Args>(args)...)
    {
    }

    /// Return local to global transform associated with this identifier
    ///
    /// @param gctx The current geometry context object, e.g. alignment
    ///
    /// @note this is called from the surface().transform(gctx)
    const Acts::Transform3D&
    transform(const Acts::GeometryContext& gctx) const final override;
  };

  inline const Acts::Transform3D&
  AlignableDetectorElement::transform(const Acts::GeometryContext& gctx) const
  {
    // cast into the right context object
    auto alignContext = std::any_cast<AlignableGeoContext>(gctx);
    Identifier::identifier_type idValue
        = Identifier::identifier_type(identifier());

    // check if we have the right alignment parameter in hand
    if (idValue < alignContext.alignmentStore.size()) {
      return alignContext.alignmentStore[idValue];
    }
    // Return the standard transform if not found
    return GenericDetectorElement::transform(gctx);
  }

}  // end of namespace Alignable
}  // end of namespace FW
