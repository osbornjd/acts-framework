// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "ACTS/Utilities/Units.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Surfaces/PlaneSurface.hpp"
#include "ACTS/Digitization/CartesianSegmentation.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"
#include "ACTS/Material/HomogeneousSurfaceMaterial.hpp"
#include "ACTS/Surfaces/SurfaceArray.hpp"
#include "ACTS/Layers/PlaneLayer.hpp"
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


/// This function builds FASER, handles shooting particles on it and collects the results. The configuration of everything is given by two blocks in this code. 
int
main(int argc, char* argv[])
{

//Define the properties of the detector

//Thickness of the detector layers
const double thicknessSCT			= 0.32 * Acts::units::_mm;
const double thicknessSupport		= 3.3 * Acts::units::_mm;
const double thickness 				= 2 * thicknessSCT + thicknessSupport;
//Material of the strips
const float X0 					= 95.7;
const float L0 					= 465.2;
const float A 					= 28.03;
const float Z 					= 14.;
const float Rho 				= 2.32e-3;
//Number of detector layers
const unsigned int numLayers 			= 5;
//Local position of the detector layers
const std::array<double, numLayers> localPos 	= {3.2 * Acts::units::_m,
						   3.4 * Acts::units::_m,
						   3.6 * Acts::units::_m,
						   3.8 * Acts::units::_m,
						   4. * Acts::units::_m};
//Position of the detector
const double posFirstSur 			= 400. * Acts::units::_m;
//Epsilon to ensure to keep the vertex and the layers inside the corresponding volumes
const double eps 				= 10. * Acts::units::_mm;
//Lorentz angle
const double lorentzangle			= 0.;
//Length of a step limited to 25m -> multiple vacuum volumes will be glued together
const int nVacs 					= 20;
//Geometry of the strips
const double stripGap				= 269. * Acts::units::_um;
const unsigned int numCells 		= 1280;
const double pitch					= 75.5 * Acts::units::_um;
const double lengthStrip			= 48.2 * Acts::units::_mm;
//x-/y-size of the setup
const double halfX				= numCells * pitch / 2;
const double halfY				= lengthStrip + stripGap / 2;
//Rotation of the strip plates
const double rotation			= 0.026;

//////////////////////////////////////////////////////////////////

//Build Surfaces
std::cout << "Building surfaces" << std::endl;

//Rectangle that contains the surface
std::shared_ptr<const Acts::RectangleBounds> recBounds(new Acts::RectangleBounds(halfX, halfY));
//Global translation of the surface
std::array<Acts::Transform3D, 2 * numLayers> t3d;
//Global rotation of the surfaces
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
                
//Material of the detector layer
Acts::MaterialProperties matProp(X0, L0, A, Z, Rho, thickness);

//Build segmentation
std::vector<float> stripBoundariesX, stripBoundariesY;
for(int iX = 0; iX <= numCells; iX++)
	stripBoundariesX.push_back(iX * pitch - (numCells * pitch) / 2);
stripBoundariesY.push_back(-lengthStrip - stripGap / 2);
stripBoundariesY.push_back(-stripGap / 2);
stripBoundariesY.push_back(stripGap / 2);
stripBoundariesY.push_back(lengthStrip + stripGap / 2);

//Build binning
Acts::BinningData binDataX(Acts::BinningOption::open, Acts::BinningValue::binX, stripBoundariesX);
std::shared_ptr<Acts::BinUtility> buX(new Acts::BinUtility(binDataX));
Acts::BinningData binDataY(Acts::BinningOption::open, Acts::BinningValue::binY, stripBoundariesY);
std::shared_ptr<Acts::BinUtility> buY(new Acts::BinUtility(binDataY));
(*buX) += (*buY);

std::shared_ptr<const Acts::Segmentation> segmentation(new Acts::CartesianSegmentation(buX, recBounds));

//Build digitization parts
std::shared_ptr<const Acts::DigitizationModule> digitizationFront(new Acts::DigitizationModule(segmentation, thickness / 2, -1, lorentzangle));
std::shared_ptr<const Acts::DigitizationModule> digitizationBack(new Acts::DigitizationModule(segmentation, thickness / 2, 1, lorentzangle));
std::array<FWGen::GenericDetectorElement*, 2 * numLayers> genDetElem;

//Put everything together in a surface
std::array<Acts::PlaneSurface*, 2 * numLayers> pSur;
//Bit nasty creation of all surfaces. In every iteration 2 surfaces (both strip detector plates of a layer) are created.
for(unsigned int iLayer = 0; iLayer < numLayers; iLayer++) 
{
	//Set an identifier. Ordered by first surface = even, second = odd number
    Identifier id(2 * iLayer);
    //Translate the surface
    t3d[2 * iLayer] = Acts::getTransformFromRotTransl(rotationPos, Acts::Vector3D(0., 0., posFirstSur + localPos[iLayer] - (thicknessSupport + thicknessSCT) / 2));
    //Add material to surface
    std::shared_ptr<Acts::SurfaceMaterial> surMat(new Acts::HomogeneousSurfaceMaterial(matProp));
    //Create digitization
    genDetElem[2 * iLayer] = new FWGen::GenericDetectorElement(id,
							    std::make_shared<const Acts::Transform3D>(t3d[2 * iLayer]),
							    recBounds,
							    thickness,
							    surMat,
							    digitizationFront);
	//Create the surface
    pSur[2 * iLayer] = new Acts::PlaneSurface(recBounds,
					    *(genDetElem[2 * iLayer]),
					    genDetElem[2 * iLayer]->identify());
					    

	//Repeat the steps for the second surface
    id = 2 * iLayer + 1;
    
    t3d[2 * iLayer + 1] = Acts::getTransformFromRotTransl(rotationNeg, Acts::Vector3D(0., 0., posFirstSur + localPos[iLayer] + (thicknessSupport + thicknessSCT) / 2));
    
    genDetElem[2 * iLayer + 1] = new FWGen::GenericDetectorElement(id,
							    std::make_shared<const Acts::Transform3D>(t3d[2 * iLayer + 1]),
							    recBounds,
							    thickness,
							    surMat,
							    digitizationBack);

    pSur[2 * iLayer + 1] = new Acts::PlaneSurface(recBounds,
					    *(genDetElem[2 * iLayer + 1]),
					    genDetElem[2 * iLayer + 1]->identify());
}

//////////////////////////////////////////////////////////////////

//Build Layers
std::cout << "Building layers" << std::endl;

std::array<Acts::Transform3D, numLayers> t3dLay;
std::array<std::unique_ptr<Acts::SurfaceArray>, numLayers> surArrays;
std::array<Acts::LayerPtr, numLayers> layPtr;
//Take 2 surfaces and put them together into a layer
for(unsigned int iSurface = 0; iSurface < numLayers; iSurface++)
{
	//Move the layer to its destination
	t3dLay[iSurface] = Acts::Translation3D(0., 0., posFirstSur + localPos[iSurface]);
	//Collect surfaces 
	Acts::SurfaceVector surVec = {pSur[2 * iSurface], pSur[2 * iSurface + 1]};
    surArrays[iSurface] = std::make_unique<Acts::SurfaceArray>(Acts::SurfaceArray(surVec));
	//Create layer
    layPtr[iSurface] = Acts::PlaneLayer::create(std::make_shared<const Acts::Transform3D>(t3dLay[iSurface]),
					recBounds,
					std::move(surArrays[iSurface]),
					thickness);
}

//////////////////////////////////////////////////////////////////

//Build tracking volume
std::cout << "Building tracking volume" << std::endl;

//Trasnlation of the volume
Acts::Transform3D trans;
trans = Acts::Translation3D(0., 0., posFirstSur + localPos.back() / 2);
std::shared_ptr<const Acts::Transform3D> htrans = std::make_shared<Acts::Transform3D>(trans);
//Create volume
Acts::VolumeBoundsPtr volBoundsPtr = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								localPos.back() / 2 + eps));
//Add material
//TODO: Does this material mean that the volume is completly filled with the material?
std::shared_ptr<const Acts::Material> mat(new Acts::Material(X0, L0, A, Z, Rho));

//Collect all layers
Acts::LayerVector layVec;
for(auto layer : layPtr) layVec.push_back(layer);

//Create layer array -> navigation through the layers
Acts::LayerArrayCreator layArrCreator(Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::VERBOSE));
std::unique_ptr<const Acts::LayerArray> layArr(layArrCreator.layerArray(
							layVec,
							posFirstSur - eps,
							posFirstSur + localPos.back() + eps,
							Acts::BinningType::arbitrary,
							Acts::BinningValue::binZ));

//Create tracking volume
Acts::MutableTrackingVolumePtr mtvp(Acts::TrackingVolume::create(htrans,
								volBoundsPtr, 
								mat, 
								std::move(layArr), 
								layVec,
								{},
								{},
								"Spurer"));

mtvp->sign(Acts::GeometrySignature::Global);


//////////////////////////////////////////////////////////////////

//Build vacuum volumes
std::cout << "Building vacuum" << std::endl;

std::array<Acts::MutableTrackingVolumePtr, nVacs> vacArr;

//Repeat the steps of the tracking volume but with vacuum and without layers
for(int iVac = 0; iVac < nVacs; iVac++)
{
	//Use epsilon around first and last vacuum -> assure everything happens in the world
	if(iVac == 0)
	{
		Acts::Transform3D transVac;
		transVac = Acts::Translation3D(0., 0., (20. * iVac + 10.)  * Acts::units::_m - eps / 2); //hardcoded
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
}

//////////////////////////////////////////////////////////////////

//Glue everything together -> allows navigation from volume to volume
for(int iVac = 0; iVac < nVacs; iVac++)
{
	if(iVac != nVacs -1)
		vacArr[iVac]->glueTrackingVolume(Acts::BoundarySurfaceFace::positiveFaceXY, vacArr[iVac + 1], Acts::BoundarySurfaceFace::negativeFaceXY);
	if(iVac != 0)
		vacArr[iVac]->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY, vacArr[iVac - 1], Acts::BoundarySurfaceFace::positiveFaceXY);
}

mtvp->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY, vacArr[nVacs - 1], Acts::BoundarySurfaceFace::positiveFaceXY);
vacArr[nVacs - 1]->glueTrackingVolume(Acts::BoundarySurfaceFace::positiveFaceXY, mtvp, Acts::BoundarySurfaceFace::negativeFaceXY);

//////////////////////////////////////////////////////////////////

//Build world
std::cout << "Building world" << std::endl;

//Translation of the world
Acts::Transform3D transWorld;
transWorld = Acts::Translation3D(0., 0., (posFirstSur + localPos.back()) / 2);
std::shared_ptr<const Acts::Transform3D> htransWorld = std::make_shared<Acts::Transform3D>(transWorld);

//Create world volume
Acts::VolumeBoundsPtr volBoundsPtrWorld = std::make_shared<const Acts::CuboidVolumeBounds>(
					Acts::CuboidVolumeBounds(halfX,
								halfY,
								(posFirstSur + localPos.back()) / 2 + eps));

//Collect the position of the subvolumes
std::vector<std::pair<std::shared_ptr<const Acts::TrackingVolume>, Acts::Vector3D>> tapVec;
tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(mtvp, Acts::Vector3D(0., 0., posFirstSur + localPos.back() / 2)));
for(int iVac = 0; iVac < nVacs; iVac++)
{
	if(iVac == 0)
	{
		tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(vacArr[iVac], Acts::Vector3D(0., 0.,  (20. * iVac + 10.)  * Acts::units::_m - eps / 2))); //hardcode
		continue;
	}
	if(iVac == nVacs - 1)
	{
		tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(vacArr[iVac], Acts::Vector3D(0., 0.,  (20. * iVac + 10.)  * Acts::units::_m + eps / 2))); //hardcode
		continue;
	}
	tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(vacArr[iVac], Acts::Vector3D(0., 0., (20. * iVac + 10.) * Acts::units::_m))); //hardcode
}

//Collect the boarders of the volumes for binning
std::vector<float> binBoundaries = {-eps};
for(int iVac = 1; iVac < nVacs; iVac++)
	binBoundaries.push_back((posFirstSur / (double) nVacs) * iVac);
binBoundaries.push_back(posFirstSur - eps);
binBoundaries.push_back(posFirstSur + localPos.back() + eps);

Acts::BinningData binData(Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
std::unique_ptr<const Acts::BinUtility> bu(new Acts::BinUtility(binData));

//Collect the volumes
std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

//Create the world
Acts::MutableTrackingVolumePtr mtvpWorld(Acts::TrackingVolume::create(htransWorld,
								volBoundsPtrWorld,
								trVolArr,
								"Welt"));

mtvpWorld->sign(Acts::GeometrySignature::Global);

//Build tracking geometry
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

//Definition about what to shoot
FW::ParticleGun::Config cfgParGun;
cfgParGun.evgenCollection = "EvgenParticles";
cfgParGun.nParticles = 5;
cfgParGun.z0Range = {{-eps / 2, eps / 2}};
cfgParGun.d0Range = {{0., 0.15 * Acts::units::_m}};
cfgParGun.etaRange = {{7., 15.}};
cfgParGun.ptRange = {{0., 10. * Acts::units::_MeV}};
cfgParGun.mass = 105.6 * Acts::units::_MeV;
cfgParGun.charge = -Acts::units::_e;
cfgParGun.pID = 13;

//Configure RNG and barcode
FW::RandomNumbersSvc::Config cfgRng;
std::shared_ptr<FW::RandomNumbersSvc> RngSvc(new FW::RandomNumbersSvc(cfgRng));
cfgParGun.randomNumbers = RngSvc;

FW::BarcodeSvc::Config cfgBarcode;
std::shared_ptr<FW::BarcodeSvc> BarSvc(new FW::BarcodeSvc(cfgBarcode));
cfgParGun.barcodes = BarSvc;

FW::RandomNumbersSvc::Config cfgEpol;

//Pass everything to the ExtrapolationExampleBase
ACTFWExtrapolationExample::run(nEvents, std::make_shared<Acts::ConstantBField>(bField), tGeo, cfgParGun, cfgEpol, Acts::Logging::VERBOSE);

for(int i = 0; i < numLayers; i++) streams[i]->close();
}





