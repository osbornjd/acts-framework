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

/// Particle identifier that encodes additional event information.
///
/// The barcode has to fulfill two separate requirements: be able to act as
/// unique identifier for particles within an event and to encode details
/// on the event structure for fast lookup. Since we only care about tracking
/// here, we only need to support two scenarios:
///
/// *   Identify which primary/secondary vertex particles belong to. No
///     information on intermediate/unstable/invisible particles needs to be
///     retained.
/// *   If visible particles convert, decay, or interact with the detector, we
///     need to be able to identify the initial (primary) particle. Typical
///     examples are pion nuclear interactions or electron/gamma conversions
///     where the leading secondary particle has similar kinematic properties
///     to the primary particle.
///
/// The vertex information is encoded as two 12bit numbers that define the
/// primary and secondary vertex. The primary vertex must be non-zero.
/// Particles with secondary vertex 0 originate directly from the primary
/// vertex.
///
/// Within one vertex (primary+secondary) each particle is identified by a
/// 16bit number. A second 16bit number stores the parent particle identifier
/// (as previously defined). A value of zero means that no visible parent
/// exists. Consequently, zero can not be a valid particle identifier for the
/// previous field and must not be used.
///
/// A 8bit process identifier provides additional information on the process
/// that created the particle.
///
/// A barcode with all elements set to zero (the default value) is an invalid
/// value that can be used e.g. to mark missing particles.
///
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

  /// Return the primary vertex identifier.
  constexpr Value
  vertexPrimary() const
  {
    return getBits(Masks::VertexPrimary);
  }
  /// Return the secondary vertex identifier.
  constexpr Value
  vertexSecondary() const
  {
    return getBits(Masks::VertexSecondary);
  }
  /// Return the particle identifier.
  constexpr Value
  particle() const
  {
    return getBits(Masks::Particle);
  }
  /// Return the parent particle identifier.
  constexpr Value
  parentParticle() const
  {
    return getBits(Masks::ParentParticle);
  }
  /// Return the process identifier.
  constexpr Value
  process() const
  {
    return getBits(Masks::Process);
  }

  /// Set the primary vertex identifier.
  constexpr Barcode&
  setVertexPrimary(Value id)
  {
    return setBits(Masks::VertexPrimary, id);
  }
  /// Set the secondary vertex identifier.
  constexpr Barcode&
  setVertexSecondary(Value id)
  {
    return setBits(Masks::VertexSecondary, id);
  }
  /// Set the particle identifier.
  constexpr Barcode&
  setParticle(Value id)
  {
    return setBits(Masks::Particle, id);
  }
  /// Set the parent particle identifier.
  constexpr Barcode&
  setParentParticle(Value id)
  {
    return setBits(Masks::ParentParticle, id);
  }
  /// Set the process identifier.
  constexpr Barcode&
  setProcess(Value id)
  {
    return setBits(Masks::Process, id);
  }

private:
  enum Masks : Value {
    VertexPrimary   = UINT64_C(0xfff0000000000000),
    VertexSecondary = UINT64_C(0x000fff0000000000),
    Particle        = UINT64_C(0x000000ffff000000),
    ParentParticle  = UINT64_C(0x0000000000ffff00),
    Process         = UINT64_C(0x00000000000000ff),
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
    os << x.vertexPrimary() << "|" << x.vertexSecondary() << "|" << x.particle()
       << "|" << x.parentParticle() << "|" << x.process();
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
