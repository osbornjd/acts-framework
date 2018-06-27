// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include "ACTFW/Utilities/SimReader.hpp"
#include "HepMC/GenEvent.h"
#include "HepMC/ReaderAscii.h"

namespace FW {

template <class R, class E>
struct SimulatedReader;

/// @struct SimulatedReader<HepMC::ReaderAscii, HepMC::GenEvent>
///
/// This structure is the explicit structure to read runs from HepMC
/// datatypes. It allows calling the functions that always handle primitive or
/// Acts data types.
///
template <>
struct SimulatedReader<HepMC::ReaderAscii, HepMC::GenEvent>
{
public:
  /// @brief Reads an event from file
  /// @param reader reader of run files
  /// @param event storage of the read event
  /// @return boolean indicator if the reading was successful
  bool
  readEvent(HepMC::ReaderAscii& reader, std::shared_ptr<HepMC::GenEvent> event);

  /// @brief Reports the status of the reader
  /// @param reader reader of run files
  /// @return boolean status indicator
  bool
  status(HepMC::ReaderAscii& reader);
};
}  // FW
