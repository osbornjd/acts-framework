// This file is part of the Acts project.
//
// Copyright (C) 2016-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/AlignableDetector/GeometryRotationDecorator.hpp"
#include "ACTFW/AlignableDetector/AlignableDetectorElement.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"

FW::Alignable::GeometryRotationDecorator::GeometryRotationDecorator(
    const FW::Alignable::GeometryRotationDecorator::Config& cfg,
    std::unique_ptr<const Acts::Logger>                     logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
  if (m_cfg.trackingGeometry != nullptr) {
    // parse and populate
    parseGeometry(*m_cfg.trackingGeometry.get());
  }
}

FW::ProcessCode
FW::Alignable::GeometryRotationDecorator::decorate(
    AlgorithmContext& context) const
{
  // Start with the nominal map
  std::vector<Acts::Transform3D> aStore = m_nominalStore;

  for (auto& tf : aStore) {
    tf *= Acts::AngleAxis3D(m_cfg.rotationStep * context.eventNumber,
                            Acts::Vector3D::UnitY());
  }

  AlignableGeoContext alignableGeoContext;
  alignableGeoContext.alignmentStore = std::move(aStore);
  context.geoContext = std::make_any<AlignableGeoContext>(alignableGeoContext);

  return ProcessCode::SUCCESS;
}

void
FW::Alignable::GeometryRotationDecorator::parseGeometry(
    const Acts::TrackingGeometry& tGeometry)
{

  // Double-visit - first count
  size_t nTransforms = 0;
  tGeometry.visitSurfaces([&nTransforms](const auto*) { ++nTransforms; });

  // Move the surfacas into the nominal store
  std::vector<Acts::Transform3D> aStore(nTransforms,
                                        Acts::Transform3D::Identity());

  auto fillTransforms = [&aStore](const auto* surface) -> void {
    auto alignableElement = dynamic_cast<const AlignableDetectorElement*>(
        surface->associatedDetectorElement());
    aStore[alignableElement->identifier()]
        = surface->transform(AlignableGeoContext());
    int i = 0;
  };

  tGeometry.visitSurfaces(fillTransforms);
  m_nominalStore = std::move(aStore);
}
