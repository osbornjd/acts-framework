// This file is part of the Acts project.
//
// Copyright (C) 2017 CERN for the benefit of the Acts project
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <iosfwd>
#include <string>
#include <vector>

using read_series  = std::vector<int>;
using read_range   = std::vector<double>;
using read_strings = std::vector<std::string>;

// Overloads must exist in the `std` namespace so ADL-lookup can find them.
namespace std {

std::ostream&
operator<<(std::ostream& os, const read_series& vec);

std::ostream&
operator<<(std::ostream& os, const read_range& vec);

std::ostream&
operator<<(std::ostream& os, const read_strings& vec);

}  // namespace std
