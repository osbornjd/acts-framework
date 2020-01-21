// This file is part of the Acts project.
//
// Copyright (C) 2018-2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>
#include <ostream>

namespace FW {

/// Particle identifier that encodes some event information.
class Barcode
{
public:
  using Value = uint64_t;

  /// Construct the barcode from an already encoded value
  constexpr Barcode(Value encoded) : m_value(encoded) {}
  /// Construct default Barcode with all values set to zero.
  Barcode()               = default;
  Barcode(Barcode&&)      = default;
  Barcode(const Barcode&) = default;
  ~Barcode()              = default;
  Barcode&
  operator=(Barcode&&)
      = default;
  Barcode&
  operator=(const Barcode&)
      = default;

  /// Return the encoded value.
  constexpr Value
  value() const
  {
    return m_value;
  }

  /// Return the vertex identifier.
  constexpr Value
  vertex() const
  {
    return getBits(Masks::Vertex);
  }
  /// Return the primary particle identifier.
  constexpr Value
  primary() const
  {
    return getBits(Masks::Primary);
  }
  /// Return the generation identifier.
  constexpr Value
  generation() const
  {
    return getBits(Masks::Generation);
  }
  /// Return the secondary particle identifier.
  constexpr Value
  secondary() const
  {
    return getBits(Masks::Secondary);
  }
  /// Return the process identifier.
  constexpr Value
  process() const
  {
    return getBits(Masks::Process);
  }

  /// Set the vertex identifier.
  constexpr Barcode&
  setVertex(Value id)
  {
    return setBits(Masks::Vertex, id);
  }
  /// Set the primary particle identifier.
  constexpr Barcode&
  setPrimary(Value id)
  {
    return setBits(Masks::Primary, id);
  }
  /// Set the generation identifier.
  constexpr Barcode&
  setGeneration(Value id)
  {
    return setBits(Masks::Generation, id);
  }
  /// Set the secondary particle identifier.
  constexpr Barcode&
  setSecondary(Value id)
  {
    return setBits(Masks::Secondary, id);
  }
  /// Set the process identifier.
  constexpr Barcode&
  setProcess(Value id)
  {
    return setBits(Masks::Process, id);
  }

private:
  enum Masks : Value {
    Vertex     = UINT64_C(0xfff0000000000000),
    Primary    = UINT64_C(0x000ffff000000000),
    Generation = UINT64_C(0x0000000fff000000),
    Secondary  = UINT64_C(0x0000000000fff000),
    Process    = UINT64_C(0x0000000000000fff),
  };

  Value m_value = 0;

  /// Extract the bit shift necessary to access the masked values.
  static constexpr int
  extractShift(Value mask)
  {
    // use compiler builtin to extract the number of trailing zero bits from the
    // mask. the builtin should be available on all supported compilers.
    // need unsigned long long version (...ll) to ensure uint64_t compatibility.
    // WARNING undefined behaviour for mask == 0 which we should not have.
    return __builtin_ctzll(mask);
  }
  /// Extract the masked bits from the encoded value.
  constexpr Value
  getBits(Value mask) const
  {
    return (m_value & mask) >> extractShift(mask);
  }
  /// Set the masked bits to id in the encoded value.
  constexpr Barcode&
  setBits(Value mask, Value id)
  {
    m_value = (m_value & ~mask) | ((id << extractShift(mask)) & mask);
    // return *this here so we need to write less lines in the set... methods
    return *this;
  }

  friend constexpr bool
  operator==(Barcode lhs, Barcode rhs)
  {
    return lhs.m_value == rhs.m_value;
  }
  friend constexpr bool
  operator<(Barcode lhs, Barcode rhs)
  {
    return lhs.m_value < rhs.m_value;
  }
  friend std::ostream&
  operator<<(std::ostream& os, Barcode x)
  {
    os << x.vertex() << "|" << x.primary() << "|" << x.generation() << "|"
       << x.secondary() << "|" << x.process();
    return os;
  }
};

}  // namespace FW

namespace std {

// specialize std::hash so the Barcode can be used e.g. in an unordered_map
template <>
struct hash<FW::Barcode>
{
  auto
  operator()(FW::Barcode idx) const noexcept
  {
    return std::hash<FW::Barcode::Value>()(idx.value());
  }
};

}  // namespace std
