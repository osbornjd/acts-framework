// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
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
  : FW::BareAlgorithm("MaterialMapping", level), m_cfg(cnf)
{
  if (!m_cfg.materialMapper) {
    throw std::invalid_argument("Missing material mapper");
  } else if (!m_cfg.indexedMaterialWriter) {
    throw std::invalid_argument("Missing indexed material writer");
  } else if (!m_cfg.trackingGeometry) {
    throw std::invalid_argument("Missing tracking geometry");
  }
}

FW::ProcessCode
FW::MaterialMapping::execute(FW::AlgorithmContext context) const
{

  const std::vector<Acts::RecordedMaterialTrack>* mtrackCollection = nullptr;

  // Write to the collection to the EventStore
  if (context.eventStore.get(m_cfg.collection, mtrackCollection)
      == FW::ProcessCode::ABORT) {
    ACTS_ERROR("Could not read the material steps from EventStore!");
    return FW::ProcessCode::ABORT;
  }

  // retrive a cache object
  auto mState = m_cfg.materialMapper->createState(*m_cfg.trackingGeometry);

  return FW::ProcessCode::SUCCESS;
}
