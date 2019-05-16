// This file is part of the Acts project.
//
// Copyright (C) 2017 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

/// @file
/// @date 2016-05-23
/// @author Andreas Salburger
/// @author Moritz Kiehnn <msmk@cern.ch>

#pragma once

#include <string>

#include "ACTFW/Framework/AlgorithmContext.hpp"
#include "ACTFW/Framework/ProcessCode.hpp"

namespace FW {

class WhiteBoard;

/// Interface for services.
///
/// A service should be used to provide constant or slowly changing
/// per-event information, e.g. alignment, and to handle once-per-run tasks.
/// In contrast to an `IAlgorithm` it can have internal state.
class IService
{
public:
  /// Virtual destructor.
  virtual ~IService() = default;

  /// Provide the name of the service.
  virtual std::string
  name() const = 0;

  /// Start-of-run hook to be called before any events are processed.
  virtual ProcessCode
  startRun()
  {
    return ProcessCode::SUCCESS;
  }

  /// Prepare per-event information.
  ///
  /// This is intended to add already existing information, e.g. geometry
  /// or conditions data, to the event store. While possible, complex
  /// operations should be better implemented as an `IAlgorithm`.
  virtual ProcessCode
  prepare(AlgorithmContext& ctx)
  {
    return ProcessCode::SUCCESS;
  }

  /// End-of-run hook to run to be called after all events are processed.
  ///
  /// @deprecated Do not use for new code.
  virtual ProcessCode
  endRun()
  {
    return ProcessCode::SUCCESS;
  }
};

}  // namespace FW
