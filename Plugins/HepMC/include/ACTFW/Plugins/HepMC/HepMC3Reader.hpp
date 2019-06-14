// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <HepMC/GenEvent.h>
#include <HepMC/ReaderAscii.h>

namespace FW {

/// HepMC3 event reader.
struct HepMC3ReaderAscii
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
}  // namespace FW
