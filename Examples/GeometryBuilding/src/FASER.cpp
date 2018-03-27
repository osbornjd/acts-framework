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

#include "ACTS/Digitization/CartesianSegmentation.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"

#include "ACTS/Material/HomogeneousSurfaceMaterial.hpp"

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
#include "../../Extrapolation/src/ExtrapolationExampleBase.hpp" //nasty but working

//TODO: includes aufraeumen


int
main(int argc, char* argv[])
{
//x-/y-size of the setup
const double halfX				= 0.2 * Acts::units::_m;
const double halfY				= 0.2 * Acts::units::_m;
//thickness of the detector layers
const double thickness 				= 10. * Acts::units::_mm;
//material of the detector layers
const float X0 					= 95.7;
const float L0 					= 465.2;
const float A 					= 28.03;
const float Z 					= 14.;
const float Rho 				= 2.32e-3;
//number of detector layers
const unsigned int numLayers 			= 5;
//local position of the detector layers
const std::array<double, numLayers> localPos 	= {1. * Acts::units::_m,
						   1.5 * Acts::units::_m,
						   2. * Acts::units::_m,
						   2.5 * Acts::units::_m,
						   3. * Acts::units::_m};
//position of the detector
const double posFirstSur 			= 25. * Acts::units::_m;
//epsilon to ensure to keep the vertex and the layers inside the corresponding volumes
const double eps 				= 10. * Acts::units::_mm;
//number of detector cells in x-/y-direction
const unsigned int numCellsX			= 2;
const unsigned int numCellsY			= 2;
//lorentz angle
const double lorentzangle			= 0.;


//Build Surfaces
std::cout << "Building surfaces" << std::endl;

//rectangle that contains the surface
std::shared_ptr<const Acts::RectangleBounds> recBounds(new Acts::RectangleBounds(halfX, halfY));
//global translation of the surface
std::array<Acts::Transform3D, numLayers> t3d;
//material of the detector layer
Acts::MaterialProperties matProp(X0, L0, A, Z, Rho, thickness);

std::shared_ptr<const Acts::Segmentation> segmentation(new Acts::CartesianSegmentation(recBounds, numCellsX, numCellsY));
std::shared_ptr<const Acts::DigitizationModule> digitization(new Acts::DigitizationModule(segmentation, thickness / 2, 1, lorentzangle)); //TODO: wenn hier die cells erzeugt werden, dann muessten alle platten die selben cells ansprechen
std::array<FWGen::GenericDetectorElement*, numLayers> genDetElem;

//putting everything together in a surface
std::array<Acts::PlaneSurface*, numLayers> pSur;
for(unsigned int iLayer = 0; iLayer < numLayers; iLayer++) 
{
    Identifier id(iLayer);
    t3d[iLayer] = Acts::Translation3D(0., 0., posFirstSur + localPos[iLayer]);
    std::shared_ptr<Acts::SurfaceMaterial> surMat(new Acts::HomogeneousSurfaceMaterial(matProp));
    
    genDetElem[iLayer] = new FWGen::GenericDetectorElement(id,
							    std::make_shared<const Acts::Transform3D>(t3d[iLayer]),
							    recBounds,
							    thickness,
							    surMat,
							    digitization);

    pSur[iLayer] = new Acts::PlaneSurface(recBounds,
					    *(genDetElem[iLayer]),
					    genDetElem[iLayer]->identify()); //TODO: identischer identifier fuer detectorelement und surface?
/*
    for(int x = 0; x < numCellsX; x++)
	for(int y = 0; y < numCellsY; y++)
	{
		const Acts::DigitizationCell dc = pSur[iLayer]->associatedDetectorElement()->digitizationModule()->cell(Acts::Vector2D((double) x, (double) y));
		std::cout << x << "," << y << "\t" << dc.channel0 << "," << dc.channel1 << "," << dc.data << std::endl;
	}
*/
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
trans = Acts::Translation3D(0., 0., posFirstSur + localPos.back() / 2);
std::shared_ptr<const Acts::Transform3D> htrans = std::make_shared<Acts::Transform3D>(trans);

Acts::VolumeBoundsPtr volBoundsPtr = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								localPos.back() / 2 + eps));

std::shared_ptr<const Acts::Material> mat(new Acts::Material(X0, L0, A, Z, Rho));

Acts::LayerVector layVec;
for(auto layer : layPtr) layVec.push_back(layer);

Acts::LayerArrayCreator layArrCreator(Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::VERBOSE));
std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
							layVec,
							posFirstSur - eps,
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
tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(mtvp, Acts::Vector3D(0., 0., posFirstSur + localPos.back() / 2))); //TODO: checken ob das passt
tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(mtvpVac, Acts::Vector3D(0., 0., posFirstSur / 2 - eps)));

const std::vector<float> binBoundaries = {-eps, posFirstSur - eps, posFirstSur + localPos.back() + eps};
Acts::BinningData binData(Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
std::unique_ptr<const Acts::BinUtility> bu(new Acts::BinUtility(binData));

std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

Acts::MutableTrackingVolumePtr mtvpWorld(Acts::TrackingVolume::create(htransWorld,
								volBoundsPtrWorld,
								trVolArr));

mtvpWorld->sign(Acts::GeometrySignature::Global);

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

//Test the setup
const unsigned nEvents = 1;
const Acts::ConstantBField bField(0., 0., 0.);

FW::ParticleGun::Config cfgParGun;
cfgParGun.evgenCollection = "EvgenParticles";
cfgParGun.nParticles = 5;
cfgParGun.z0Range = {{-eps / 2, eps / 2}};
cfgParGun.d0Range = {{0., 0.15 * Acts::units::_m}};
cfgParGun.etaRange = {{0., 10.}};
cfgParGun.ptRange = {{0., 10. * Acts::units::_MeV}};
cfgParGun.mass = 105.6 * Acts::units::_MeV;
cfgParGun.charge = - Acts::units::_e;
cfgParGun.pID = 13;

FW::RandomNumbersSvc::Config cfgRng;
std::shared_ptr<FW::RandomNumbersSvc> RngSvc(new FW::RandomNumbersSvc(cfgRng));
cfgParGun.randomNumbers = RngSvc;

FW::BarcodeSvc::Config cfgBarcode;
std::shared_ptr<FW::BarcodeSvc> BarSvc(new FW::BarcodeSvc(cfgBarcode));
cfgParGun.barcodes = BarSvc;

FW::RandomNumbersSvc::Config cfgEpol;

ACTFWExtrapolationExample::run(nEvents, std::make_shared<Acts::ConstantBField>(bField), tGeo, cfgParGun, cfgEpol, Acts::Logging::VERBOSE);

for(int i = 0; i < numLayers; i++) streams[i]->close();
//TODO: Digitization als methode in extrapolationexamplebase implementieren (kommt aus fatrascommon.hpp/.cpp
}





