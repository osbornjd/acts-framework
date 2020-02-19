// This file is part of the Acts project.
//
// Copyright (C) 2019 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @brief Plain structs that each define one row in a TrackML csv file

#pragma once

#include <cstdint>

#include <dfe/dfe_namedtuple.hpp>

namespace FW {

struct ParticleData
{
  uint64_t particle_id;
  int32_t  particle_type;
  /// defaulted, process type is not available in the TrackML datasets
  uint32_t process = 0u;
  float    vx, vy, vz;
  // defaulted, time is not available in the TrackML datasets
  float vt = 0.0f;
  float px, py, pz;
  // defaulted, mass is not available in the TrackML datasets
  float m = 0.0f;
  float q;

  DFE_NAMEDTUPLE(ParticleData,
                 particle_id,
                 particle_type,
                 process,
                 vx,
                 vy,
                 vz,
                 vt,
                 px,
                 py,
                 pz,
                 m,
                 q);
};

struct TruthHitData
{
  uint64_t hit_id;
  uint64_t particle_id;
  // defaulted, geometry identifier is not available in the TrackML datasets
  uint64_t geometry_id = 0u;
  float    tx, ty, tz;
  // defaulted, time is not available in the TrackML datasets
  float tt = 0.0f;
  float tpx, tpy, tpz;
  // defaulted, particle energy is not available in the TrackML datasets
  float te = 0.0f;
  // defaulted, hit index is not available in the TrackML datasets
  int32_t index = -1;

  DFE_NAMEDTUPLE(TruthHitData,
                 hit_id,
                 particle_id,
                 geometry_id,
                 tx,
                 ty,
                 tz,
                 tt,
                 tpx,
                 tpy,
                 tpz,
                 te,
                 index);
};

struct HitData
{
  uint64_t hit_id;
  // defaulted, geometry identifier is not available in the TrackML datasets
  uint64_t geometry_id = 0u;
  uint32_t volume_id, layer_id, module_id;
  float    x, y, z;
  // defaulted, time is not available in the TrackML datasets
  float t = 0.0f;

  DFE_NAMEDTUPLE(HitData,
                 hit_id,
                 geometry_id,
                 volume_id,
                 layer_id,
                 module_id,
                 x,
                 y,
                 z,
                 t);
};

struct CellData
{
  uint64_t hit_id;
  // These should have been named channel{0,1} but we cannot change it now
  // to avoid breaking backward compatibility.
  int32_t ch0, ch1;
  // defaulted, timestamp is not available in the TrackML datasets
  int32_t timestamp = 0;
  int32_t value;

  DFE_NAMEDTUPLE(CellData, hit_id, ch0, ch1, timestamp, value);
};

struct SurfaceData
{
  uint64_t geometry_id;
  uint32_t volume_id, layer_id, module_id;
  float    cx, cy, cz;
  float    rot_xu, rot_xv, rot_xw;
  float    rot_yu, rot_yv, rot_yw;
  float    rot_zu, rot_zv, rot_zw;
  // Limits and pitches are not always available and need invalid defaults.
  float module_t     = -1;
  float module_minhu = -1;
  float module_maxhu = -1;
  float module_hv    = -1;
  float pitch_u      = -1;
  float pitch_v      = -1;

  DFE_NAMEDTUPLE(SurfaceData,
                 geometry_id,
                 volume_id,
                 layer_id,
                 module_id,
                 cx,
                 cy,
                 cz,
                 rot_xu,
                 rot_xv,
                 rot_xw,
                 rot_yu,
                 rot_yv,
                 rot_yw,
                 rot_zu,
                 rot_zv,
                 rot_zw,
                 module_t,
                 module_minhu,
                 module_maxhu,
                 module_hv,
                 pitch_u,
                 pitch_v);
};

}  // namespace FW
