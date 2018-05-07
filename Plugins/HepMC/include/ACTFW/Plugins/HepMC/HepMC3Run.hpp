// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <fstream>
#include "ACTFW/Plugins/HepMC/HepMC3Event.hpp"
#include "HepMC/GenEvent.h"
#include "HepMC/ReaderAscii.h"
#include "HepPID/ParticleIDMethods.hh"

namespace FW {

/// @class HepMC3Run
///
/// This class reads a HepMC3 file (run) and stores it in the object.
///
class HepMC3Run
{
public:
  /// @brief Constructor
  /// @param filename name of the HepMC file that will be read
  HepMC3Run(const std::string filename);

  /// @brief Reads an event from file
  /// @return boolean indicator if the reading was successful
  bool
  readEvent();

  /// @brief Reads the whole run from file
  void
  readRun();

  /// @brief Getter of the amount of events read
  /// @return number of events read
  unsigned int
  nEvents();

  /// @brief Getter of a single event
  /// @param index index of the event that will be returned
  /// @return event with index @p index
  const std::shared_ptr<HepMC3Event>&
  event(unsigned int index);

  /// @brief Getter of all read events
  /// @return collection of all read events
  const std::vector<std::shared_ptr<HepMC3Event>>&
  events();

private:
  /// Internal file reader
  HepMC::ReaderAscii m_reader;
  /// Storage of read events
  std::vector<std::shared_ptr<HepMC3Event>> m_events;
};
}  // FW
