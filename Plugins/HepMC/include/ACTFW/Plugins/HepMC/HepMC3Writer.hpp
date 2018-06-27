// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Utilities/SimWriter.hpp"
#include "HepMC/GenEvent.h"
#include "HepMC/WriterAscii.h"

namespace FW {

template <class W, class E>
struct SimulatedWriter;

/// @struct SimulatedWriter<HepMC::WriterAscii, HepMC::GenEvent>
///
/// This structure is the explicit structure to write runs in HepMC
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <>
struct SimulatedWriter<HepMC::WriterAscii, HepMC::GenEvent>
{
public:
  /// @brief Writes an event to file
  /// @param writer writer of run files
  /// @param event storage of the event
  /// @return boolean indicator if the writing was successful
  /// @note HepMC3 does not state a success or failure. The returned argument is
  /// always true.
  bool
  writeEvent(HepMC::WriterAscii&              writer,
             std::shared_ptr<HepMC::GenEvent> event);

  /// @brief Reports the status of the writer
  /// @param writer writer of run files
  /// @return boolean status indicator
  bool
  status(HepMC::WriterAscii& writer);
};
}  // FW
