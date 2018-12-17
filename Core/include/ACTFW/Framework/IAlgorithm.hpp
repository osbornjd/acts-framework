// This file is part of the Acts project.
//
// Copyright (C) 2017-2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

/// Interface for event processing algorithms.
class IAlgorithm
{
public:
  /// Virtual destructor
  virtual ~IAlgorithm() = default;

  /// Framework name() method
  virtual std::string
  name() const = 0;

  /// Framework execute method
  virtual ProcessCode
  execute(AlgorithmContext context) const = 0;
};

}  // namespace FW
