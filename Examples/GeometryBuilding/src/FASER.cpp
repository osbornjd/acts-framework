// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>

#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Surfaces/PlaneSurface.hpp"

//#include "ACTS/Surfaces/SurfaceArray.hpp"
#include "ACTS/Surfaces/concept/AnySurfaceGridLookup.hpp"
#include "ACTS/Layers/PlaneLayer.hpp"

namespace po = boost::program_options;

int
main(int argc, char* argv[])
{

  // Declare the supported program options.
  po::options_description desc("Allowed options");
  desc.add_options()("help", "Produce help message")(
      "sloglevel",
      po::value<size_t>()->default_value(2),
      "The output log level for surfaces")(
      "lloglevel",
      po::value<size_t>()->default_value(2),
      "The output log level for surfaces")(
      "vloglevel",
      po::value<size_t>()->default_value(2),
      "The output log level for surfaces");

  po::variables_map vm;
  // Get all options from contain line and store it into the map
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  // print help if requested
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
//---------------------------------------------------------

double halfX = 5;
double halfY = 5;
int num_Layers = 5;
std::vector<double> distances(num_Layers);

SurfaceVector surVec(num_Layers);
for(int i = 0; i < num_Layers; i++)
{
    const Acts::RectangleBounds rbounds(halfX, halfY);

    Acts::Transform3D t3d;
    t3d = Acts::Translation3D(0., 0., 150.); //TODO:Units

    const Acts::PlaneSurface ps(std::make_shared<const Acts::Transform3D>(t3d), std::make_shared<const Acts::RectangleBounds>(rbounds));
    surVec.push_back(&ps);
}
Acts::concepts::AnySurfaceGridLookup<SurfaceVector> lookup;

Acts::SurfaceArray surArray(SurVec, surVec);
//Acts::PlaneLayer player;
}
