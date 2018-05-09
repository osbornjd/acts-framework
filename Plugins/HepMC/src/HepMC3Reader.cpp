// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Reader.hpp"

bool
FW::SimulatedReader<HepMC::ReaderAscii, HepMC::GenEvent>::readEvent(
    HepMC::ReaderAscii&              reader,
    std::shared_ptr<HepMC::GenEvent> event)
{
  // Read event and store it
  if (reader.read_event(*event)) return true;
  return false;
}

bool
FW::SimulatedReader<HepMC::ReaderAscii, HepMC::GenEvent>::readRun(
    HepMC::ReaderAscii&                            reader,
    std::vector<std::shared_ptr<HepMC::GenEvent>>& events)
{
  std::shared_ptr<HepMC::GenEvent> event;
  // Read single event
  if (!readEvent(reader, event)) return false;
  // Continue reading events as long as the reading is successful
  do
    events.push_back(event);
  while (readEvent(reader, event));
  return true;
}
