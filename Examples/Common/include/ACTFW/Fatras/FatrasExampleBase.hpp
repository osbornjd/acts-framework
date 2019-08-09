// This file is part of the Acts project.
//
// Copyright (C) 2017-2019 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

namespace FW {
class IBaseDetector;
}

/// @brief The Fatras example
///
/// This instantiates the geometry and runs fast track simultion
///
/// @param argc the number of argumetns of the call
/// @param argv the argument list
/// @param detector The detector instance
int
fatrasExample(int argc, char* argv[], FW::IBaseDetector& detector);
