// This file is part of the Acts project.
//
// Copyright (C) 2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <cstdint>

#include <dfe/dfe_namedtuple.hpp>

namespace FW {

struct ParticleData
{
  uint64_t particle_id;
  int32_t  particle_type;
  float    x, y, z, t;
  float    px, py, pz;
  float    q;
  int32_t  nhits;

  DFE_NAMEDTUPLE(ParticleData,
                 particle_id,
                 particle_type,
                 x,
                 y,
                 z,
                 t,
                 px,
                 py,
                 pz,
                 q,
                 nhits);
};

struct HitData
{
  uint64_t hit_id;
  float    x, y, z, t;
  uint32_t volume_id, layer_id, module_id;

  DFE_NAMEDTUPLE(HitData, hit_id, x, y, z, t, volume_id, layer_id, module_id);
};

struct CellData
{
  uint64_t hit_id;
  // These should have been named channel{0,1} but we cannot change it now
  // to avoid breaking backward compatibility.
  int32_t  ch0, ch1;
  int32_t  timestamp;
  int32_t  value;

  DFE_NAMEDTUPLE(CellData, hit_id, ch0, ch1, timestamp, value);
};

struct TruthData
{
  uint64_t hit_id;
  uint64_t particle_id;
  float    tx, ty, tz, tt;
  float    tpx, tpy, tpz;

  DFE_NAMEDTUPLE(TruthData, hit_id, particle_id, tx, ty, tz, tt, tpx, tpy, tpz);
};

}  // namespace FW
