// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>

using read_series  = std::vector<int>;
using read_range   = std::vector<double>;
using read_strings = std::vector<std::string>;

namespace std {

std::ostream&
operator<<(std::ostream& os, const read_series& vec)
{
  for (auto item : vec) {
    os << item << " ";
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, const read_range& vec)
{
  for (auto item : vec) {
    os << item << " ";
  }
  return os;
}

std::ostream&
operator<<(std::ostream& os, const read_strings& vec)
{
  for (auto item : vec) {
    os << item << " ";
  }
  return os;
}
}
