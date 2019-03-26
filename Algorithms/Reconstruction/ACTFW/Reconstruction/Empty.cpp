// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "Empty.hpp"

#include <stdexcept>

#include <Acts/Utilities/Definitions.hpp>
#include <Acts/Utilities/GeometryID.hpp>

#include "ACTFW/EventData/DataContainers.hpp"
#include "ACTFW/Framework/WhiteBoard.hpp"

FW::EmptyReconstructionAlgorithm::EmptyReconstructionAlgorithm(
    const FW::EmptyReconstructionAlgorithm::Config& cfg,
    Acts::Logging::Level                            logLevel)
  : BareAlgorithm("EmptyReconstructionAlgorithm", logLevel), m_cfg(cfg)
{
  if (m_cfg.spacePointCollection.empty()) {
    throw std::invalid_argument("Missing input space points collection");
  }
}

FW::ProcessCode
FW::EmptyReconstructionAlgorithm::execute(FW::AlgorithmContext ctx) const
{
  ACTS_INFO("empty reconstruction on event " << ctx.eventNumber);

  // Read the input space points
  const DetectorData<geo_id_value, Acts::Vector3D>* spacePoints;
  if (ctx.eventStore.get(m_cfg.spacePointCollection, spacePoints)
      == FW::ProcessCode::ABORT) {
    ACTS_WARNING("missing space point input");
    return FW::ProcessCode::ABORT;
  }

  for (auto& volumeData : *spacePoints) {
    for (auto& layerData : volumeData.second) {
      for (auto& moduleData : layerData.second) {
        ACTS_INFO("volume " << volumeData.first << " layer " << layerData.first
                            << " module " << moduleData.first << " "
                            << moduleData.second.size() << " space points");
        for (auto& cluster : moduleData.second) {
          // TODO do something w/ the space points and write out track
          // candidates NOTE space points are grouped by the geometry id
        }
      }
    }
  }

  return ProcessCode::SUCCESS;
}
