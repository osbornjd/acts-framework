// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/Json/JsonMaterialWriter.hpp"
#include <fstream>
#include <ios>
#include <iostream>
#include <stdexcept>
#include "Acts/Material/BinnedSurfaceMaterial.hpp"
#include "Acts/Utilities/GeometryID.hpp"

FW::Json::JsonMaterialWriter::JsonMaterialWriter(
    const FW::Json::JsonGeometryConverter::Config& cfg)
{
  // Validate the configuration
  if (m_cfg.name.empty()) {
    throw std::invalid_argument("Missing service name");
  }
}

FW::Json::JsonMaterialWriter::~JsonMaterialWriter()
{
}

FW::ProcessCode
FW::Json::JsonMaterialWriter::write(
    const Acts::DetectorMaterialMaps& detMaterial)
{

  FW::Json::JsonGeometryConverter jmConverter(m_cfg);

  auto jout = jmConverter.materialMapsToJson(detMaterial);
  // write prettified JSON to another file
  std::string   jsonOutputName = m_cfg.fileName;
  std::ofstream ofj(jsonOutputName);
  ofj << std::setw(4) << jout << std::endl;

  // return success
  return FW::ProcessCode::SUCCESS;
}
