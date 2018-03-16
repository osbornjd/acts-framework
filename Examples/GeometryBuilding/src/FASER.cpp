// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <boost/program_options.hpp>

#include "ACTS/Utilities/Units.hpp"

#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Utilities/Definitions.hpp"
#include "ACTS/Surfaces/PlaneSurface.hpp"

#include "ACTS/Surfaces/SurfaceArray.hpp"

#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Layers/PlaneLayer.hpp"

#include "ACTS/Tools/ITrackingVolumeHelper.hpp"
#include "ACTS/Volumes/CuboidVolumeBounds.hpp"
#include "ACTS/Material/Material.hpp"

#include "ACTS/Tools/LayerArrayCreator.hpp"

#include "ACTS/Utilities/BinningType.hpp"

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

//TODO: Units
const double halfX = 			5. * Acts::units::_m;
const double halfY = 			5. * Acts::units::_m;
const double thickness = 		0. * Acts::units::_mm;
const float X0 = 			95.7;
const float L0 = 			465.2;
const float A = 			28.03;
const float Z = 			14.;
const float Rho = 			2.32e-3;
const unsigned int numLayers = 	5;
std::array<double, numLayers> localPos =		{0. * Acts::units::_mm,
				5. * Acts::units::_mm,
				10. * Acts::units::_mm,
				15. * Acts::units::_mm,
				20. * Acts::units::_mm};
double posFirstSur =		150. * Acts::units::_m;


//Build Surfaces
std::shared_ptr<const Acts::RectangleBounds> recBounds(new Acts::RectangleBounds(halfX, halfY));

std::array<Acts::Transform3D, numLayers> t3d;
std::array<Acts::PlaneSurface*, numLayers> pSur;
for(unsigned int iLayer = 0; iLayer < numLayers; iLayer++) 
{
    t3d[iLayer] = Acts::Translation3D(0., 0., posFirstSur + localPos[iLayer]);

    pSur[iLayer] = new Acts::PlaneSurface(
			std::make_shared<const Acts::Transform3D>(t3d[iLayer]), recBounds);
}

//Build Layers
std::array<std::unique_ptr<Acts::SurfaceArray>, numLayers> surArrays;
std::array<Acts::LayerPtr, numLayers> layPtr;
for(unsigned int iSurface; iSurface < numLayers; iSurface++)
{
    surArrays[iSurface] = std::make_unique<Acts::SurfaceArray>(Acts::SurfaceArray(pSur[iSurface]));

    layPtr[iSurface] = Acts::PlaneLayer::create(std::make_shared<const Acts::Transform3D>(t3d[iSurface]),
					recBounds,
					std::move(surArrays[iSurface]),
					thickness);
}

//Build Volumes
Acts::Transform3D trans;
trans = Acts::Translation3D(0., 0., posFirstSur + (localPos.front() + localPos.back()) / 2);
std::shared_ptr<const Acts::Transform3D> htrans(&trans);

Acts::VolumeBoundsPtr volBoundsPtr = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								localPos.front() + localPos.back()));

std::shared_ptr<const Acts::Material> mat(new Acts::Material(X0, L0, A, Z, Rho));


Acts::LayerVector layVec;
for(auto layer : layPtr) layVec.push_back(layer);

Acts::LayerArrayCreator layArrCreator;
std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
							layVec,
							posFirstSur + localPos.front(), //TODO: Epsilon Umgebung
							posFirstSur + localPos.back(),
							Acts::BinningType::arbitrary,
							Acts::BinningValue::binZ));

Acts::MutableTrackingVolumePtr mtvp(Acts::TrackingVolume::create(htrans,
								volBoundsPtr, 
								mat, 
								std::move(layArr), 
								layVec));

//Build vacuum volume
Acts::Transform3D transVac;
transVac = Acts::Translation3D(0., 0., 0.);
std::shared_ptr<const Acts::Transform3D> htransVac(&transVac);

Acts::VolumeBoundsPtr volBoundsPtrVac = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								localPos.front()));

std::shared_ptr<const Acts::Material> matVac(new Acts::Material());
Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(htransVac,
								volBoundsPtrVac,
								matVac);





Acts::TrackingGeometry tGeo(mtvp);

//---------------------------------------------------------


}





