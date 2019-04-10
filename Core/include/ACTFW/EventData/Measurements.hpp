// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/EventData/RawData.hpp"
#include "Acts/EventData/Measurement.hpp"

namespace FW {
namespace Data {
    // Framework Measurement definition
    using Measurement = Acts::FittableMeasurement<RawData>;
    
    // The measurements that are in use currently
    using MeasurementL0  
        = Acts::Measurement_t<RawData, ParDef::eLOC_0>;
    using MeasurementL1  
        = Acts::Measurement_t<RawData, ParDef::eLOC_1>;
    using MeasurementL0L1 
        = Acts::Measurement_t<RawData, ParDef::eLOC_0, ParDef::eLOC_1>;
    
}
}