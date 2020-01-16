// This file is part of the Acts project.
//
// Copyright (C) 2018-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>

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
    barcode_type barcode = 0;
    // create the barcode w/ all components
    barcode = setBits(barcode, m_cfg.vertex_mask, vertex);
    barcode = setBits(barcode, m_cfg.primary_mask, primary);
    barcode = setBits(barcode, m_cfg.generation_mask, generation);
    barcode = setBits(barcode, m_cfg.secondary_mask, secondary);
    barcode = setBits(barcode, m_cfg.process_mask, process);
    return barcode;
  }

  /// Decode the vertex number.
  barcode_type
  vertex(barcode_type barcode) const
  {
    return getBits(barcode, m_cfg.vertex_mask);
  }

  /// Decode the primary index.
  barcode_type
  primary(barcode_type barcode) const
  {
    return getBits(barcode, m_cfg.primary_mask);
  }

  /// Decode the generation number.
  barcode_type
  generation(barcode_type barcode) const
  {
    return getBits(barcode, m_cfg.generation_mask);
  }

  /// Decode the secondary index.
  barcode_type
  secondary(barcode_type barcode) const
  {
    return getBits(barcode, m_cfg.secondary_mask);
  }

  /// Decode the process number.
  barcode_type
  process(barcode_type barcode) const
  {
    return getBits(barcode, m_cfg.process_mask);
  }

private:
  /// Extract the bit shift necessary to access the masked values.
  static constexpr int
  extractShift(barcode_type mask)
  {
    // use compiler builtin to extract the number of trailing zero bits from the
    // mask. the builtin should be available on all supported compilers.
    // need unsigned long long version (...ll) to ensure uint64_t compatibility.
    // WARNING undefined behaviour for mask == 0 which we should not have.
    return __builtin_ctzll(mask);
  }
  /// Extract the masked bits from the encoded value.
  static constexpr barcode_type
  getBits(barcode_type value, barcode_type mask)
  {
    return (value & mask) >> extractShift(mask);
  }
  /// Set the masked bits to id in the encoded value.
  static constexpr barcode_type
  setBits(barcode_type value, barcode_type mask, barcode_type id)
  {
    return (value & ~mask) | ((id << extractShift(mask)) & mask);
  }

  Config m_cfg;
};

}  // namespace FW
