// This file is part of the ACTS project.
//
// Copyright (C) 2018 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>
#include <memory>
#include "ACTFW/Plugins/HepMCReader/HepMCReader.hpp"
#include <fstream>


namespace po = boost::program_options;


int
main(int argc, char* argv[])
{
  FW::HepMCReader hepmcr;
  hepmcr.print();
  
  
}
