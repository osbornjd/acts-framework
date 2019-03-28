// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/ContextualDetector/AlignmentDecorator.hpp"
#include "Acts/Detector/TrackingGeometry.hpp"

FW::Contextual::AlignmentDecorator::AlignmentDecorator(
    const FW::Contextual::AlignmentDecorator::Config& cfg,
    std::unique_ptr<const Acts::Logger>               logger)
  : m_cfg(cfg), m_logger(std::move(logger))
{
}

FW::ProcessCode
FW::Contextual::AlignmentDecorator::decorate(AlgorithmContext& context) const
{

  if (not(context.eventNumber % m_cfg.iovSize)) {
    // screen output
    ACTS_VERBOSE("New IOV detected at event " << context.eventNumber
                                              << ", emulate new alignment.");
    // set the correct iov
    unsigned int iov         = 0;
    bool         iovAssigned = false;
    // flush event detection
    bool flushEvent = !(context.eventNumber % m_cfg.flushSize);
    if (flushEvent) {
      ACTS_VERBOSE("Flush detected at event "
                   << context.eventNumber
                   << ", garbage collection (to be implemented).");
    }

    for (auto& lstore : m_cfg.detectorStore) {
      for (auto& ldet : lstore) {
        // get the nominal transform
        auto& tForm = ldet->nominalTransform(context.geoContext);
        // todo: now modify & and add
        auto atForm = std::make_unique<Acts::Transform3D>(tForm);
        if (!iovAssigned) {
          iov         = ldet->alignedTransforms().size();
          iovAssigned = true;
        }
        // put it back into the store
        ldet->addAlignedTransform(std::move(atForm), flushEvent);
      }
    }
    // Screen output
    ACTS_VERBOSE("New IOV identifier set to " << iov);
    // set the geometry context
    AlignedDetectorElement::ContextType alignedContext{iov};
    context.geoContext
        = std::make_any<AlignedDetectorElement::ContextType>(alignedContext);
  }

  return ProcessCode::SUCCESS;
}
