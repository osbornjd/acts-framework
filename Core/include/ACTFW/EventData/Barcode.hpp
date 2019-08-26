// This file is part of the Acts project.
//
// Copyright (C) 2018-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>

#include <Acts/Utilities/Helpers.hpp>

namespace FW {

/// Particle barcode.
using barcode_type = uint64_t;

/// Encode and decode particle barcodes.
class BarcodeSvc
{
public:
  struct Config
  {
    barcode_type vertex_mask     = 0xfff0000000000000;
    barcode_type primary_mask    = 0x000ffff000000000;
    barcode_type generation_mask = 0x0000000fff000000;
    barcode_type secondary_mask  = 0x0000000000fff000;
    barcode_type process_mask    = 0x0000000000000fff;
  };

  BarcodeSvc(const Config& cfg) : m_cfg(cfg) {}

  /// Encode a new barcode.
  barcode_type
  generate(barcode_type vertex     = 0,
           barcode_type primary    = 0,
           barcode_type generation = 0,
           barcode_type secondary  = 0,
           barcode_type process    = 0) const
  {
    // create the barcode w/ all components
    barcode_type barcode = 0;
    barcode += ACTS_BIT_ENCODE(vertex, m_cfg.vertex_mask);
    barcode += ACTS_BIT_ENCODE(primary, m_cfg.primary_mask);
    barcode += ACTS_BIT_ENCODE(generation, m_cfg.generation_mask);
    barcode += ACTS_BIT_ENCODE(secondary, m_cfg.secondary_mask);
    barcode += ACTS_BIT_ENCODE(process, m_cfg.process_mask);
    return barcode;
  }

  /// Decode the vertex number.
  barcode_type
  vertex(barcode_type barcode) const
  {
    return ACTS_BIT_DECODE(barcode, m_cfg.vertex_mask);
  }

  /// Decode the primary index.
  barcode_type
  primary(barcode_type barcode) const
  {
    return ACTS_BIT_DECODE(barcode, m_cfg.primary_mask);
  }

  /// Decode the generation number.
  barcode_type
  generation(barcode_type barcode) const
  {
    return ACTS_BIT_DECODE(barcode, m_cfg.generation_mask);
  }

  /// Decode the secondary index.
  barcode_type
  secondary(barcode_type barcode) const
  {
    return ACTS_BIT_DECODE(barcode, m_cfg.secondary_mask);
  }

  /// Decode the process number.
  barcode_type
  process(barcode_type barcode) const
  {
    return ACTS_BIT_DECODE(barcode, m_cfg.process_mask);
  }

private:
  Config m_cfg;
};

}  // namespace FW
