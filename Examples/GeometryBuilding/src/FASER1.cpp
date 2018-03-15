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

#include "ACTS/Surfaces/SurfaceArray.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Layers/PlaneLayer.hpp"

#include "ACTS/Tools/ITrackingVolumeHelper.hpp"
#include "ACTS/Volumes/CuboidVolumeBounds.hpp"
#include "ACTS/Material/Material.hpp"
#include "ACTS/Utilities/BinnedArrayXD.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"

#include "ACTS/Detector/TrackingGeometry.hpp"

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

double halfX = 5.;
double halfY = 5.;
double thickness = 0.;
float X0 = 95.7;
float L0 = 465.2;
float A = 28.03;
float Z = 14.;
float Rho = 2.32e-3;


const Acts::RectangleBounds rbounds(halfX, halfY);

Acts::Transform3D t3d;
t3d = Acts::Translation3D(0., 0., 150.); //TODO:Units, Location

Acts::PlaneSurface ps(std::make_shared<const Acts::Transform3D>(t3d), std::make_shared<const Acts::RectangleBounds>(rbounds));
Acts::SurfaceArray surArray(&ps);
std::shared_ptr<Acts::RectangleBounds> recBounds(new Acts::RectangleBounds(halfX, halfY));

Acts::LayerPtr mlp(Acts::PlaneLayer::create(std::make_shared<const Acts::Transform3D>(t3d), recBounds, std::make_unique<Acts::SurfaceArray>(surArray), thickness));

Acts::VolumeBoundsPtr vbp(new Acts::CuboidVolumeBounds(halfX, halfY, 0.5 * thickness));
std::shared_ptr<Acts::Material> mat(new Acts::Material(X0, L0, A, Z, Rho));

std::unique_ptr<const Acts::LayerArray> binArrXD(new Acts::BinnedArrayXD<Acts::LayerPtr>(mlp));

const Acts::LayerVector layVec(1, mlp);


//TODO: Transform3D direkt als shared_ptr erzeugen
Acts::MutableTrackingVolumePtr mtvp(Acts::TrackingVolume::create(std::make_shared<const Acts::Transform3D>(t3d), vbp, mat, std::move(binArrXD), layVec));

Acts::TrackingGeometry tGeo(mtvp);

//---------------------------------------------------------


}





