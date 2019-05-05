// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

///////////////////////////////////////////////////////////////////
// MaterialMapping.cpp
///////////////////////////////////////////////////////////////////

#include "ACTFW/MaterialMapping/MaterialMapping.hpp"
#include <iostream>
#include <stdexcept>
#include "ACTFW/Framework/WhiteBoard.hpp"

FW::MaterialMapping::MaterialMapping(const FW::MaterialMapping::Config& cnf,
                                     Acts::Logging::Level               level)
  : FW::BareAlgorithm("MaterialMapping", level)
  , m_cfg(cnf)
  , m_mappingState(cnf.geoContext, cnf.magFieldContext)
{
  if (!m_cfg.materialMapper) {
    throw std::invalid_argument("Missing material mapper");
  } else if (!m_cfg.trackingGeometry) {
    throw std::invalid_argument("Missing tracking geometry");
  }

  ACTS_INFO("This algorithm requires inter-event information, "
            << "run in single-threaded mode!");

  // Generate and retrieve the central cache object
  m_mappingState = m_cfg.materialMapper->createState(
      m_cfg.geoContext, m_cfg.magFieldContext, *m_cfg.trackingGeometry);
}

FW::MaterialMapping::~MaterialMapping()
{

  // Finalize all the maps using the cached state
  m_cfg.materialMapper->finalizeMaps(m_mappingState);

  // Create a fake context for writing
  AlgorithmContext writeContext(0, 0, m_whiteBoard);
  writeContext.geoContext      = m_cfg.geoContext;
  writeContext.magFieldContext = m_cfg.magFieldContext;

  Acts::DetectorMaterialMaps detectorMaterial;

  // Loop over the state, and collect the maps for surfaces
  for (auto & [ key, value ] : m_mappingState.surfaceMaterial) {
    detectorMaterial.first.insert({key, std::move(value)});
  }

  for (auto& imw : m_cfg.materialWriters) {
    imw->write(writeContext, detectorMaterial);
  }
}

FW::ProcessCode
FW::MaterialMapping::execute(const FW::AlgorithmContext& context) const
{

  const std::vector<Acts::RecordedMaterialTrack>* mtrackCollection = nullptr;

  // Write to the collection to the EventStore
  if (context.eventStore.get(m_cfg.collection, mtrackCollection)
      == FW::ProcessCode::ABORT) {
    ACTS_ERROR("Could not read the material steps from EventStore!");
    return FW::ProcessCode::ABORT;
  }

  // To make it work with the framework needs a lock guard
  auto mappingState
      = const_cast<Acts::SurfaceMaterialMapper::State*>(&m_mappingState);

  for (auto mTrack : (*mtrackCollection)) {
    // Map this one onto the geometry
    m_cfg.materialMapper->mapMaterialTrack(*mappingState, mTrack);
  }

  return FW::ProcessCode::SUCCESS;
}
