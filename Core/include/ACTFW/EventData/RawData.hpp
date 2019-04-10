// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/EventData/SimIdentifier.hpp"
#include "Acts/Plugins/Digitization/DigitizationCell.hpp"

namespace FW {
namespace Data {

    using Identifier = SimIdentifier;
    using CellData   = std::vector<DigitizationCell>;
    
    /// @brief the RawData of the Digitization
    /// 
    /// It contains the link to the Identifier and to the raw input of 
    /// the digitization it acts as the source_link_t template in measurements
    struct RawData {
        Identifier identifier;
        CellData   cellData;
    };

}
}