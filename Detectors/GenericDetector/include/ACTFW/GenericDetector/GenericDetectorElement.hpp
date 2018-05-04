// This file is part of the ACTS project.
//
// Copyright (C) 2016 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef AGD_GENERICDETECTORELEMENT_GENERICDETECTORELEMENT
#define AGD_GENERICDETECTORELEMENT_GENERICDETECTORELEMENT 1

#include "Acts/Detector/DetectorElementBase.hpp"
#include "Acts/Utilities/Definitions.hpp"
#include "Acts/Utilities/Identifier.hpp"

namespace Acts {
class Surface;
class PlanarBounds;
class DiscBounds;
class SurfaceMaterial;
class DigitizationModule;
}

namespace FWGen {

/// @class GenericDetectorElement
///
/// This is a lightweight type of detector element,
/// it simply implements the base class.
///
class GenericDetectorElement : public Acts::DetectorElementBase
{
public:
  /// Constructor for single sided detector element
  /// - bound to a Plane Surface
  ///
  /// @param identifier is the module identifier
  /// @param transform is the transform that element the layer in 3D frame
  /// @param pBounds is the planar bounds for the planar detector element
  /// @param thickness is the module thickness
  /// @param material is the (optional) Surface material associated to it
  GenericDetectorElement(const Identifier                          identifier,
                         std::shared_ptr<const Acts::Transform3D>  transform,
                         std::shared_ptr<const Acts::PlanarBounds> pBounds,
                         double                                    thickness,
                         std::shared_ptr<const Acts::SurfaceMaterial> material
                         = nullptr,
                         std::shared_ptr<const Acts::DigitizationModule> dModule
                         = nullptr);

  /// Constructor for single sided detector element
  /// - bound to a Disc Surface
  ///
  /// @param identifier is the module identifier
  /// @param transform is the transform that element the layer in 3D frame
  /// @param dBounds is the planar bounds for the disc like detector element
  /// @param thickness is the module thickness
  /// @param material is the (optional) Surface material associated to it
  GenericDetectorElement(const Identifier                         identifier,
                         std::shared_ptr<const Acts::Transform3D> transform,
                         std::shared_ptr<const Acts::DiscBounds>  dBounds,
                         double                                   thickness,
                         std::shared_ptr<const Acts::SurfaceMaterial> material
                         = nullptr);

  /// Identifier
  Identifier
  identify() const final override;

  /// Return local to global transform associated with this identifier
  ///
  /// @note this is called from the surface().transform() in the PROXY mode
  ///
  /// @param identifier is ignored for this simple detector element
  const Acts::Transform3D&
  transform(const Identifier& identifier = Identifier()) const final override;

  /// Return surface associated with this identifier,
  ///
  /// @param identifier is ignored in this case
  ///
  /// @param identifier is ignored for this simple detector element
  const Acts::Surface&
  surface(const Identifier& identifier = Identifier()) const final override;

  /// Returns the full list of all detection surfaces associated
  /// to this detector element
  const std::vector<std::shared_ptr<const Acts::Surface>>&
  surfaces() const final override;

  /// Return the DigitizationModule
  /// @return optionally the DigitizationModule
  std::shared_ptr<const Acts::DigitizationModule>
  digitizationModule() const final override;

  /// Set the identifier after construction (sometimes needed)
  void
  assignIdentifier(const Identifier& identifier) final override;

  /// The maximal thickness of the detector element wrt normal axis
  double
  thickness() const final override;

private:
  /// the element representation
  /// identifier
  Identifier m_elementIdentifier;
  /// the transform for positioning in 3D space
  std::shared_ptr<const Acts::Transform3D> m_elementTransform;
  /// the surface represented by it
  std::shared_ptr<const Acts::Surface> m_elementSurface;
  /// the element thickness
  double m_elementThickness;

  /// the cache for the surfaces
  std::vector<std::shared_ptr<const Acts::Surface>> m_elementSurfaces;
  /// store either
  std::shared_ptr<const Acts::PlanarBounds> m_elementPlanarBounds;
  std::shared_ptr<const Acts::DiscBounds>   m_elementDiscBounds;

  // the digitization module, it's shared because many
  // elements could potentiall have the same readout infrastructure
  std::shared_ptr<const Acts::DigitizationModule> m_digitizationModule;
};

inline std::shared_ptr<const Acts::DigitizationModule>
FWGen::GenericDetectorElement::digitizationModule() const
{
  return m_digitizationModule;
}

inline void
FWGen::GenericDetectorElement::assignIdentifier(const Identifier& identifier)
{
  m_elementIdentifier = identifier;
}

inline Identifier
FWGen::GenericDetectorElement::identify() const
{
  return m_elementIdentifier;
}

inline const Acts::Transform3D&
FWGen::GenericDetectorElement::transform(const Identifier&) const
{
  return *m_elementTransform;
}

inline const Acts::Surface&
FWGen::GenericDetectorElement::surface(const Identifier&) const
{
  return *m_elementSurface;
}

inline const std::vector<std::shared_ptr<const Acts::Surface>>&
FWGen::GenericDetectorElement::surfaces() const
{
  return m_elementSurfaces;
}

inline double
FWGen::GenericDetectorElement::thickness() const
{
  return m_elementThickness;
}

}  // end of ns

#endif
