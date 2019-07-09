// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>
#include <vector>

namespace FW {

/// Join dir and name into one paths with correct handling of empty dirs.
std::string
joinPaths(const std::string& dir, const std::string& name);

/// Construct a file path of the form `[<dir>/]event<XXXXX>-<name>`.
///
/// @params dir output directory, unused if empty
/// @params name basic filename
/// @params event event number
std::string
perEventFilepath(const std::string& dir, const std::string& name, size_t event);

/// Brief split a file list into a vector of file names to add
///
/// @param files The joint file list
/// @param tag The splitting tag
std::vector<std::string>
splitByDelimiter(const std::string& files, char delim);

}  // namespace FW
