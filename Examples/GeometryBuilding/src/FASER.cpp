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
#include "../ExtrapolationExampleBase.hpp" //nasty but working

//TODO: includes aufraeumen


int
main(int argc, char* argv[])
{

//thickness of the detector layers
const double thicknessSCT			= 0.32 * Acts::units::_mm;
const double thicknessSupport		= 3.3 * Acts::units::_mm;
const double thickness 				= 2 * thicknessSCT + thicknessSupport;
//material of the detector layers
const float X0 					= 95.7;
const float L0 					= 465.2;
const float A 					= 28.03;
const float Z 					= 14.;
const float Rho 				= 2.32e-3;
//number of detector layers
const unsigned int numLayers 			= 5;
//local position of the detector layers
const std::array<double, numLayers> localPos 	= {3.2 * Acts::units::_m,
						   3.4 * Acts::units::_m,
						   3.6 * Acts::units::_m,
						   3.8 * Acts::units::_m,
						   4. * Acts::units::_m};
//position of the detector
const double posFirstSur 			= 400. * Acts::units::_m;
//epsilon to ensure to keep the vertex and the layers inside the corresponding volumes
const double eps 				= 10. * Acts::units::_mm;
//lorentz angle
const double lorentzangle			= 0.;
const int nVacs 					= 20;
const double stripGap				= 269. * Acts::units::_um;
const unsigned int numCells 		= 1280;
const double pitch					= 75.5 * Acts::units::_um;
const double lengthStrip			= 48.2 * Acts::units::_mm;
//x-/y-size of the setup
const double halfX				= numCells * pitch / 2;
const double halfY				= lengthStrip + stripGap / 2;
const double rotation			= 0.026;

//Build Surfaces
std::cout << "Building surfaces" << std::endl;

//rectangle that contains the surface
std::shared_ptr<const Acts::RectangleBounds> recBounds(new Acts::RectangleBounds(halfX, halfY));
//global translation of the surface
std::array<Acts::Transform3D, 2 * numLayers> t3d;

Acts::RotationMatrix3D rotationPos, rotationNeg;

Acts::Vector3D xPos(cos(rotation), sin(rotation), 0.);
Acts::Vector3D yPos(-sin(rotation), cos(rotation), 0.);
Acts::Vector3D zPos(0., 0., 1.);
rotationPos.col(0) = xPos;
rotationPos.col(1) = yPos;
rotationPos.col(2) = zPos;

Acts::Vector3D xNeg(cos(-rotation), sin(-rotation), 0.);
Acts::Vector3D yNeg(-sin(-rotation), cos(-rotation), 0.);
Acts::Vector3D zNeg(0., 0., 1.);
rotationNeg.col(0) = xNeg;
rotationNeg.col(1) = yNeg;
rotationNeg.col(2) = zNeg;                
                
//material of the detector layer
Acts::MaterialProperties matProp(X0, L0, A, Z, Rho, thickness);

//Build Segmentation
std::vector<float> stripBoundariesX, stripBoundariesY;
for(int iX = 0; iX <= numCells; iX++)
	stripBoundariesX.push_back(iX * pitch - (numCells * pitch) / 2);
stripBoundariesY.push_back(-lengthStrip - stripGap / 2);
stripBoundariesY.push_back(-stripGap / 2);
stripBoundariesY.push_back(stripGap / 2);
stripBoundariesY.push_back(lengthStrip + stripGap / 2);

Acts::BinningData binDataX(Acts::BinningOption::open, Acts::BinningValue::binX, stripBoundariesX);
std::shared_ptr<Acts::BinUtility> buX(new Acts::BinUtility(binDataX));
Acts::BinningData binDataY(Acts::BinningOption::open, Acts::BinningValue::binY, stripBoundariesY);
std::shared_ptr<Acts::BinUtility> buY(new Acts::BinUtility(binDataY));
(*buX) += (*buY);

std::shared_ptr<const Acts::Segmentation> segmentation(new Acts::CartesianSegmentation(buX, recBounds));

std::shared_ptr<const Acts::DigitizationModule> digitization(new Acts::DigitizationModule(segmentation, thickness / 2, 1, lorentzangle)); //TODO: readout richtung festlegen
std::array<FWGen::GenericDetectorElement*, 2 * numLayers> genDetElem;

//putting everything together in a surface
std::array<Acts::PlaneSurface*, 2 * numLayers> pSur;
for(unsigned int iLayer = 0; iLayer < numLayers; iLayer++) 
{
    Identifier id(2 * iLayer);
    
    t3d[2 * iLayer] = Acts::getTransformFromRotTransl(rotationPos, Acts::Vector3D(0., 0., posFirstSur + localPos[iLayer] - (thicknessSupport + thicknessSCT) / 2));
    
    std::shared_ptr<Acts::SurfaceMaterial> surMat(new Acts::HomogeneousSurfaceMaterial(matProp));
    
    genDetElem[2 * iLayer] = new FWGen::GenericDetectorElement(id,
							    std::make_shared<const Acts::Transform3D>(t3d[2 * iLayer]),
							    recBounds,
							    thickness,
							    surMat,
							    digitization);

    pSur[2 * iLayer] = new Acts::PlaneSurface(recBounds,
					    *(genDetElem[2 * iLayer]),
					    genDetElem[2 * iLayer]->identify());
					    

    id = 2 * iLayer + 1;
    
    t3d[2 * iLayer + 1] = Acts::getTransformFromRotTransl(rotationNeg, Acts::Vector3D(0., 0., posFirstSur + localPos[iLayer] + (thicknessSupport + thicknessSCT) / 2));
    
    genDetElem[2 * iLayer + 1] = new FWGen::GenericDetectorElement(id,
							    std::make_shared<const Acts::Transform3D>(t3d[2 * iLayer + 1]),
							    recBounds,
							    thickness,
							    surMat,
							    digitization);

    pSur[2 * iLayer + 1] = new Acts::PlaneSurface(recBounds,
					    *(genDetElem[2 * iLayer + 1]),
					    genDetElem[2 * iLayer + 1]->identify());
}

//Build Layers
std::cout << "Building layers" << std::endl;

std::array<Acts::Transform3D, numLayers> t3dLay;
std::array<std::unique_ptr<Acts::SurfaceArray>, numLayers> surArrays;
std::array<Acts::LayerPtr, numLayers> layPtr;
for(unsigned int iSurface = 0; iSurface < numLayers; iSurface++)
{
	t3dLay[iSurface] = Acts::Translation3D(0., 0., posFirstSur + localPos[iSurface]);
	
	Acts::SurfaceVector surVec = {pSur[2 * iSurface], pSur[2 * iSurface + 1]};
	
    surArrays[iSurface] = std::make_unique<Acts::SurfaceArray>(Acts::SurfaceArray(surVec));
	
    layPtr[iSurface] = Acts::PlaneLayer::create(std::make_shared<const Acts::Transform3D>(t3dLay[iSurface]),
					recBounds,
					std::move(surArrays[iSurface]),
					thickness);
    //~ std::cout << iSurface << "\t" << layPtr[iSurface]->surfaceArray()->size() << "\t" << (layPtr[iSurface]->surfaceArray()->surfaces()).size() << std::endl;
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

std::array<Acts::MutableTrackingVolumePtr, nVacs> vacArr;

for(int iVac = 0; iVac < nVacs; iVac++)
{
	if(iVac == 0)
	{
		Acts::Transform3D transVac;
		transVac = Acts::Translation3D(0., 0., (20. * iVac + 10.)  * Acts::units::_m - eps / 2); //hardcode
		std::shared_ptr<const Acts::Transform3D> htransVac = std::make_shared<Acts::Transform3D>(transVac);
		
		Acts::VolumeBoundsPtr volBoundsPtrVac = std::make_shared<const Acts::CuboidVolumeBounds>(
							Acts::CuboidVolumeBounds(halfX,
										halfY,
										(posFirstSur / (double) nVacs + eps) / 2));
		
		Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(htransVac,
										volBoundsPtrVac,
										nullptr,
										"Vakuum" + std::to_string(iVac)));
		
		mtvpVac->sign(Acts::GeometrySignature::Global);
		vacArr[iVac] = mtvpVac;
		//~ std::cout << iVac << "\t" << (20. * iVac + 10.)  * Acts::units::_m - eps / 2 << "\t" << (posFirstSur / (double) nVacs + eps) / 2 << std::endl;
		continue;
	}
	
	if(iVac == nVacs - 1)
	{
		Acts::Transform3D transVac;
		transVac = Acts::Translation3D(0., 0., (20. * iVac + 10.)  * Acts::units::_m + eps / 2); //hardcode
		std::shared_ptr<const Acts::Transform3D> htransVac = std::make_shared<Acts::Transform3D>(transVac);
		
		Acts::VolumeBoundsPtr volBoundsPtrVac = std::make_shared<const Acts::CuboidVolumeBounds>(
							Acts::CuboidVolumeBounds(halfX,
										halfY,
										(posFirstSur / (double) nVacs + eps) / 2));
		
		Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(htransVac,
										volBoundsPtrVac,
										nullptr,
										"Vakuum" + std::to_string(iVac)));
		
		mtvpVac->sign(Acts::GeometrySignature::Global);
		vacArr[iVac] = mtvpVac;
		//~ std::cout << iVac << "\t" << (20. * iVac + 10.)  * Acts::units::_m + eps / 2 << "\t" << (posFirstSur / (double) nVacs + eps) / 2 << std::endl;
		continue;
	}
	
	Acts::Transform3D transVac;
	transVac = Acts::Translation3D(0., 0., (20. * iVac + 10.) * Acts::units::_m); //hardcode
	std::shared_ptr<const Acts::Transform3D> htransVac = std::make_shared<Acts::Transform3D>(transVac);
	
	Acts::VolumeBoundsPtr volBoundsPtrVac = std::make_shared<const Acts::CuboidVolumeBounds>(
						Acts::CuboidVolumeBounds(halfX,
									halfY,
									(posFirstSur / (double) nVacs) / 2));
	
	Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(htransVac,
									volBoundsPtrVac,
									nullptr,
									"Vakuum" + std::to_string(iVac)));
	
	mtvpVac->sign(Acts::GeometrySignature::Global);
	vacArr[iVac] = mtvpVac;
	//~ std::cout << iVac << "\t" << (20. * iVac + 10.) * Acts::units::_m << "\t" << (posFirstSur / (double) nVacs) / 2 << std::endl;
}

//Glue everything together
for(int iVac = 0; iVac < nVacs; iVac++)
{
	if(iVac != nVacs -1)
		vacArr[iVac]->glueTrackingVolume(Acts::BoundarySurfaceFace::positiveFaceXY, vacArr[iVac + 1], Acts::BoundarySurfaceFace::negativeFaceXY);
	if(iVac != 0)
		vacArr[iVac]->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY, vacArr[iVac - 1], Acts::BoundarySurfaceFace::positiveFaceXY);
}

mtvp->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY, vacArr[nVacs - 1], Acts::BoundarySurfaceFace::positiveFaceXY);
vacArr[nVacs - 1]->glueTrackingVolume(Acts::BoundarySurfaceFace::positiveFaceXY, mtvp, Acts::BoundarySurfaceFace::negativeFaceXY);

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
tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(mtvp, Acts::Vector3D(0., 0., posFirstSur + localPos.back() / 2)));
for(int iVac = 0; iVac < nVacs; iVac++)
{
	if(iVac == 0)
	{
		tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(vacArr[iVac], Acts::Vector3D(0., 0.,  (20. * iVac + 10.)  * Acts::units::_m - eps / 2)));
		continue;
	}
	if(iVac == nVacs - 1)
	{
		tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(vacArr[iVac], Acts::Vector3D(0., 0.,  (20. * iVac + 10.)  * Acts::units::_m + eps / 2)));
		continue;
	}
	tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(vacArr[iVac], Acts::Vector3D(0., 0., (20. * iVac + 10.) * Acts::units::_m)));
}

std::vector<float> binBoundaries = {-eps};
for(int iVac = 1; iVac < nVacs; iVac++)
	binBoundaries.push_back((posFirstSur / (double) nVacs) * iVac);
binBoundaries.push_back(posFirstSur - eps);
binBoundaries.push_back(posFirstSur + localPos.back() + eps);

Acts::BinningData binData(Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
std::unique_ptr<const Acts::BinUtility> bu(new Acts::BinUtility(binData));

std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

Acts::MutableTrackingVolumePtr mtvpWorld(Acts::TrackingVolume::create(htransWorld,
								volBoundsPtrWorld,
								trVolArr,
								"Welt"));

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
const unsigned nEvents = 10;
const Acts::ConstantBField bField(0., 0., 0.);

FW::ParticleGun::Config cfgParGun;
cfgParGun.evgenCollection = "EvgenParticles";
cfgParGun.nParticles = 100;
cfgParGun.z0Range = {{-eps / 2, eps / 2}};
cfgParGun.d0Range = {{0., 0.15 * Acts::units::_m}};
cfgParGun.etaRange = {{7., 15.}};
cfgParGun.ptRange = {{0., 10. * Acts::units::_MeV}};
cfgParGun.mass = 105.6 * Acts::units::_MeV;
cfgParGun.charge = -Acts::units::_e;
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
}





