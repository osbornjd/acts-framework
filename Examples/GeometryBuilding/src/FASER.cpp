// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

//#include <boost/program_options.hpp>

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

#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"

#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "../../Extrapolation/src/ExtrapolationExampleBase.hpp"

//namespace po = boost::program_options;


int
main(int argc, char* argv[])
{
/*
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
*/
//---------------------------------------------------------

//TODO: Units
const double halfX				= 5. * Acts::units::_m;
const double halfY				= 5. * Acts::units::_m;
const double thickness 				= 5. * Acts::units::_mm;
const float X0 					= 95.7;
const float L0 					= 465.2;
const float A 					= 28.03;
const float Z 					= 14.;
const float Rho 				= 2.32e-3;
const unsigned int numLayers 			= 5;
const std::array<double, numLayers> localPos 	= {5. * Acts::units::_m,
						   10. * Acts::units::_m,
						   15. * Acts::units::_m,
						   20. * Acts::units::_m,
						   25. * Acts::units::_m};
const double posFirstSur 			= 5. * Acts::units::_m; //TODO: modifiziert
const double eps 				= 10. * Acts::units::_mm; //Muss jedes layer + thickness beinhalten


//Build Surfaces
std::cout << "Building surfaces" << std::endl;

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

std::cout << "Building layers" << std::endl;

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

std::cout << "Building tracking volume" << std::endl;

Acts::Transform3D trans;
trans = Acts::Translation3D(0., 0., posFirstSur + (localPos.back() - localPos.front()) / 2);
std::shared_ptr<const Acts::Transform3D> htrans = std::make_shared<Acts::Transform3D>(trans);

Acts::VolumeBoundsPtr volBoundsPtr = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								(localPos.back() - localPos.front()) / 2 + eps));

std::shared_ptr<const Acts::Material> mat(new Acts::Material(X0, L0, A, Z, Rho));


Acts::LayerVector layVec;
for(auto layer : layPtr) layVec.push_back(layer);

Acts::LayerArrayCreator layArrCreator(Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::VERBOSE));
std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
							layVec,
							posFirstSur + localPos.front() - eps,
							posFirstSur + localPos.back() + eps,
							Acts::BinningType::arbitrary,
							Acts::BinningValue::binZ));

Acts::MutableTrackingVolumePtr mtvp(Acts::TrackingVolume::create(htrans,
								volBoundsPtr, 
								mat, 
								std::move(layArr), 
								layVec,
								{},
								{},
								"Spurer"));

mtvp->sign(Acts::GeometrySignature::Global);

std::cout << "Spurervolumen: " << posFirstSur + localPos.front() - eps << "\t" << posFirstSur + localPos.back() + eps << "\t" << (localPos.back() - localPos.front()) / 2 + eps << "\t" << posFirstSur + (localPos.back() - localPos.front()) / 2 << std::endl;

//Build vacuum volume
std::cout << "Building vacuum" << std::endl;

Acts::Transform3D transVac;
transVac = Acts::Translation3D(0., 0., posFirstSur / 2 - eps);
std::shared_ptr<const Acts::Transform3D> htransVac = std::make_shared<Acts::Transform3D>(transVac);

Acts::VolumeBoundsPtr volBoundsPtrVac = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								posFirstSur / 2));

Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(htransVac,
								volBoundsPtrVac,
								nullptr,
								"Vakuum"));

std::cout << "Vakuumvolumen: " << posFirstSur / 2 << "\t" << posFirstSur / 2 - eps << std::endl;

mtvpVac->sign(Acts::GeometrySignature::Global);

//Glue everything together
mtvp->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY, mtvpVac, Acts::BoundarySurfaceFace::positiveFaceXY);
mtvpVac->glueTrackingVolume(Acts::BoundarySurfaceFace::positiveFaceXY, mtvp, Acts::BoundarySurfaceFace::negativeFaceXY);

//Build world
std::cout << "Building world" << std::endl;

Acts::Transform3D transWorld;
transWorld = Acts::Translation3D(0., 0., (posFirstSur + localPos.back()) / 2);
std::shared_ptr<const Acts::Transform3D> htransWorld = std::make_shared<Acts::Transform3D>(transWorld);

Acts::VolumeBoundsPtr volBoundsPtrWorld = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								(posFirstSur + localPos.back()) / 2 + eps));

std::vector<std::pair<std::shared_ptr<const Acts::TrackingVolume>, Acts::Vector3D>> tapVec;
tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(mtvp, Acts::Vector3D(0., 0., posFirstSur + (localPos.back() - localPos.front()) / 2))); //TODO: checken ob das passt
tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(mtvpVac, Acts::Vector3D(0., 0., posFirstSur / 2 - eps)));

const std::vector<float> binBoundaries = {-eps, posFirstSur - eps, posFirstSur + localPos.back() + eps};
Acts::BinningData binData(Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
std::unique_ptr<const Acts::BinUtility> bu(new Acts::BinUtility(binData));

std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

Acts::MutableTrackingVolumePtr mtvpWorld(Acts::TrackingVolume::create(htransWorld,
								volBoundsPtrWorld,
								trVolArr));

mtvpWorld->sign(Acts::GeometrySignature::Global);

std::cout << "Weltvolumen: " << (posFirstSur + localPos.back()) / 2 + eps << "\t" << (posFirstSur + localPos.back()) / 2 << std::endl;

for(unsigned int i = 0; i < 181; i += 10) 
    std::cout << i << "\t" << mtvpWorld->trackingVolume(Acts::Vector3D(0., 0., i * Acts::units::_m))->volumeName() << std::endl;

auto tGeo = std::shared_ptr<Acts::TrackingGeometry>(new Acts::TrackingGeometry(mtvpWorld));


//Produce .obj file(s)
std::cout << "Writing output" << std::endl;

FWObj::ObjSurfaceWriter::Config cfgSur;
std::vector<std::shared_ptr<std::ofstream>> streams;
std::vector<std::shared_ptr<FWObj::ObjSurfaceWriter>> objSurWriter;
for(unsigned int iSurface = 0; iSurface < numLayers; iSurface++)
{
    cfgSur = FWObj::ObjSurfaceWriter::Config(std::to_string(iSurface), Acts::Logging::DEBUG);
    cfgSur.filePrefix = std::to_string(iSurface) + std::string(".obj");

    streams.push_back(std::shared_ptr<std::ofstream>(new std::ofstream));
    streams.back()->open(cfgSur.filePrefix);

    cfgSur.outputStream = streams.back();

    objSurWriter.push_back(std::shared_ptr<FWObj::ObjSurfaceWriter>(new FWObj::ObjSurfaceWriter(cfgSur)));
    
    objSurWriter.back()->write(*(pSur[iSurface]));
}

std::cout << mtvpVac->geometrySignature() << std::endl;

//Test the setup
const unsigned nEvents = 1;
const Acts::ConstantBField bField(0., 0., 0.); //TODO: auf 0 gesetzt, kann/sollte modifiziert werden

FW::ParticleGun::Config cfgParGun;
cfgParGun.evgenCollection = "EvgenParticles";
cfgParGun.nParticles = 10000;
cfgParGun.z0Range = {{-eps / 2 * Acts::units::_mm, eps / 2 * Acts::units::_mm}};
cfgParGun.etaRange = {{-10., 10.}};
cfgParGun.ptRange = {{0., 10. * Acts::units::_GeV}};
cfgParGun.mass = 511 * Acts::units::_keV;
cfgParGun.charge = - Acts::units::_e;
cfgParGun.pID = 11;

FW::RandomNumbersSvc::Config cfgRng;
std::shared_ptr<FW::RandomNumbersSvc> RngSvc(new FW::RandomNumbersSvc(cfgRng));
cfgParGun.randomNumbers = RngSvc;

FW::BarcodeSvc::Config cfgBarcode;
std::shared_ptr<FW::BarcodeSvc> BarSvc(new FW::BarcodeSvc(cfgBarcode));
cfgParGun.barcodes = BarSvc;

FW::RandomNumbersSvc::Config cfgEpol;

ACTFWExtrapolationExample::run(nEvents, std::make_shared<Acts::ConstantBField>(bField), tGeo, cfgParGun, cfgEpol, Acts::Logging::VERBOSE);

/*
FWObj::ObjTrackingGeometryWriter::Config cfgTGeo("TrackingGeo", Acts::Logging::DEBUG);

cfgTGeo.surfaceWriters = objSurWriter;

//cfgTGeo.filePrefix = "mtllib material.mtl\n"; //nutzlos
//cfgTGeo.sensitiveGroupPrefix = "usemtl silicon\n";
//cfgTGeo.layerPrefix = "usemtl support\n";

FWObj::ObjTrackingGeometryWriter objTGeoWriter(cfgTGeo);

objTGeoWriter.write(tGeo);
*/
//---------------------------------------------------------
for(int i = 0; i < numLayers; i++) streams[i]->close();

}





