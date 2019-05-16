// This file is part of the Acts project.
//
// Copyright (C) 2018 Acts project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTFW/Plugins/HepMC/HepMC3Writer.hpp"

bool
FW::HepMC3WriterAscii::writeEvent(HepMC::WriterAscii&              writer,
                                  std::shared_ptr<HepMC::GenEvent> event)
{
  // Write event from storage
  writer.write_event(*event);
  return true;
}

bool
FW::HepMC3WriterAscii::status(HepMC::WriterAscii& writer)
{
  return writer.failed();
}
