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

/// Event data reader interface.
///
/// Read data from disk and add it to the event store. The reader can have
/// internal state and implementations are responsible to handle concurrent
/// calls.
class IReader
{
public:
  virtual ~IReader() = default;

  /// The reader name.
  virtual std::string
  name() const = 0;

  /// Provide the maximum number of events available or SIZE_MAX if undefined.
  virtual size_t
  numEvents() const = 0;

  /// Read data for the requested event and write it into the event store.
  ///
  /// As a result of the parallelization and/or skipping events, this method
  /// will most likely not be called in order. Implementations must use the
  /// event number provided to select the proper data to be read.
  virtual ProcessCode
  read(const AlgorithmContext& context)
      = 0;
};

}  // namespace FW
