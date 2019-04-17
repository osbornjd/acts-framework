// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Json/JsonProtoMaterialReader.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/finder.hpp>
#include <boost/algorithm/string/iter_find.hpp>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Acts/Material/ProtoSurfaceMaterial.hpp"
#include "Acts/Utilities/BinUtility.hpp"
#include "Acts/Utilities/BinningType.hpp"
#include "Acts/Utilities/GeometryID.hpp"

FW::Json::JsonProtoSurfaceMaterialReader::JsonProtoSurfaceMaterialReader(
    const FW::Json::JsonProtoSurfaceMaterialReader::Config& cfg)
  : FW::IReaderT<Acts::SurfaceMaterialMap>(), m_cfg(cfg)
{
  // Validate the configuration
  if (m_cfg.fileName.empty()) {
    throw std::invalid_argument("Missing file name");
  } else if (!m_cfg.logger) {
    throw std::invalid_argument("Missing logger");
  }
}

FW::ProcessCode
FW::Json::JsonProtoSurfaceMaterialReader::read(
    Acts::SurfaceMaterialMap& sMaterialMap,
    size_t /*skip*/,
    const FW::AlgorithmContext* /*ctx*/)
{
  // lock the mutex
  std::lock_guard<std::mutex> lock(m_read_mutex);

  // sMaterialMap[geoID] = std::move(sMaterial);

  // Announce success
  return FW::ProcessCode::SUCCESS;
}
