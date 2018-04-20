// This file is part of the ACTS project.
//
// Copyright (C) 2017 ACTS project team
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include "../ExtrapolationExampleBase.hpp"  //nasty but working
#include "ACTFW/Barcode/BarcodeSvc.hpp"
#include "ACTFW/GenericDetector/GenericDetectorElement.hpp"
#include "ACTFW/ParticleGun/ParticleGun.hpp"
#include "ACTFW/Plugins/Obj/ObjSurfaceWriter.hpp"
#include "ACTFW/Plugins/Obj/ObjTrackingGeometryWriter.hpp"
#include "ACTFW/Random/RandomNumbersSvc.hpp"
#include "ACTS/Detector/TrackingGeometry.hpp"
#include "ACTS/Detector/TrackingVolume.hpp"
#include "ACTS/Digitization/CartesianSegmentation.hpp"
#include "ACTS/Digitization/DigitizationModule.hpp"
#include "ACTS/Layers/PlaneLayer.hpp"
#include "ACTS/MagneticField/ConstantBField.hpp"
#include "ACTS/Material/HomogeneousSurfaceMaterial.hpp"
#include "ACTS/Material/Material.hpp"
#include "ACTS/Surfaces/PlaneSurface.hpp"
#include "ACTS/Surfaces/RectangleBounds.hpp"
#include "ACTS/Surfaces/SurfaceArray.hpp"
#include "ACTS/Tools/LayerArrayCreator.hpp"
#include "ACTS/Utilities/BinnedArrayXD.hpp"
#include "ACTS/Utilities/BinningType.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "ACTS/Volumes/CuboidVolumeBounds.hpp"

#include <fstream>
#include <stdio.h> 
#include <stdlib.h>

// Number of detector layers
const unsigned int numLayers = 5;
// Length of a step limited to 25m -> multiple vacuum volumes will be glued
// together
const int nVacs = 20;
  
struct configParams {
	
  // Thickness of the detector layers
  double thicknessSCT     = 0.32 * Acts::units::_mm;
  double thicknessSupport = 3.3 * Acts::units::_mm;
  double thickness        = 2 * thicknessSCT + thicknessSupport;
  // Material of the strips
  float X0  = 95.7;
  float L0  = 465.2;
  float A   = 28.03;
  float Z   = 14.;
  float Rho = 2.32e-3;
  // Local position of the detector layers
  std::array<double, numLayers> localPos = {3.2 * Acts::units::_m,
                                                  3.4 * Acts::units::_m,
                                                  3.6 * Acts::units::_m,
                                                  3.8 * Acts::units::_m,
                                                  4. * Acts::units::_m};
  // Position of the detector
  double posFirstSur = 400. * Acts::units::_m;
  // Epsilon to ensure to keep the vertex and the layers inside the
  // corresponding volumes
  float eps = 10. * Acts::units::_mm;
  // Lorentz angle
  double lorentzangle = 0.;
  // Geometry of the strips
  double       stripGap    = 269. * Acts::units::_um;
  unsigned int numCells    = 1280;
  double       pitch       = 75.5 * Acts::units::_um;
  double       lengthStrip = 48.2 * Acts::units::_mm;
  // x-/y-size of the setup
  double halfX = numCells * pitch / 2;
  double halfY = lengthStrip + stripGap / 2;
  // Rotation of the strip plates
  double rotation = 0.026;
  
  void readConfig(char* filename)
  {
	 std::ifstream ifs(filename, std::fstream::in);
     std::string line;
	 
	 if(getline(ifs, line))
	  thicknessSCT = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
	 if(getline(ifs, line))
	  thicknessSupport = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
	 if(getline(ifs, line))
	  X0 = atof(line.substr(line.find(" ") + 1).c_str());
	 if(getline(ifs, line))
	  L0 = atof(line.substr(line.find(" ") + 1).c_str());
	 if(getline(ifs, line))
	  A = atof(line.substr(line.find(" ") + 1).c_str());
	 if(getline(ifs, line))
	  Z = atof(line.substr(line.find(" ") + 1).c_str());
	 if(getline(ifs, line))
	  Rho = atof(line.substr(line.find(" ") + 1).c_str());
	 if(getline(ifs, line))
	  localPos[0] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
	 if(getline(ifs, line))
	  localPos[1] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
	 if(getline(ifs, line))
	  localPos[2] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
	 if(getline(ifs, line))
	  localPos[3] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
	 if(getline(ifs, line))
	  localPos[4] = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
	 if(getline(ifs, line))
	  posFirstSur = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_m;
	 if(getline(ifs, line))
	  eps = atof(line.substr(line.find(" ") + 1).c_str()) * Acts::units::_mm;
	 if(getline(ifs, line))
	  lorentzangle = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
	 if(getline(ifs, line))
	  stripGap = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_um;
	 if(getline(ifs, line))
	  numCells = atoi(line.substr(line.find(" ") + 1).c_str());
	 if(getline(ifs, line))
	  pitch = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_um;
	 if(getline(ifs, line))
	  lengthStrip = strtod(line.substr(line.find(" ") + 1).c_str(), NULL) * Acts::units::_mm;
	 if(getline(ifs, line))
	  rotation = strtod(line.substr(line.find(" ") + 1).c_str(), NULL);
	 
	 halfX = numCells * pitch / 2;
     halfY = lengthStrip + stripGap / 2; 
     thickness        = 2 * thicknessSCT + thicknessSupport;
     
	 ifs.close();
  }
  
  void printConfig()
  {
	std::cout << "thicknessSCT: " << thicknessSCT << std::endl;  
	std::cout << "thicknessSupport: " << thicknessSupport << std::endl;
	std::cout << "thickness: " << thickness << std::endl;
	std::cout << "X0: " << X0 << std::endl;
	std::cout << "L0: " << L0 << std::endl;
	std::cout << "A: " << A << std::endl;
	std::cout << "Z: " << Z << std::endl;
	std::cout << "Rho: " << Rho << std::endl;
	std::cout << "localPos: " << localPos[0] << " " << localPos[1] << " "<< localPos[2] << " "<< localPos[3] << " "<< localPos[4] << std::endl;
	std::cout << "posFirstSur: " << posFirstSur << std::endl;
	std::cout << "eps: " << eps << std::endl;
	std::cout << "lorentzangle: " << lorentzangle << std::endl;
	std::cout << "stripGap: " << stripGap << std::endl;
	std::cout << "numCells: " << numCells << std::endl;
	std::cout << "pitch: " << pitch << std::endl;
	std::cout << "lengthStrip: " << lengthStrip << std::endl;
	std::cout << "halfX: " << halfX << std::endl;
	std::cout << "halfY: " << halfY << std::endl;
	std::cout << "rotation: " << rotation << std::endl;
  }
};

std::array<Acts::LayerPtr, numLayers> buildLayers(configParams& cfg, std::shared_ptr<const Acts::RectangleBounds>& recBounds, std::array<Acts::PlaneSurface*, 2 * numLayers>& pSur)           
{
  std::array<Acts::Transform3D, numLayers>                   t3dLay;
  std::array<std::unique_ptr<Acts::SurfaceArray>, numLayers> surArrays;
  std::array<Acts::LayerPtr, numLayers>                      layPtr;
  // Take 2 surfaces and put them together into a layer
  for (unsigned int iSurface = 0; iSurface < numLayers; iSurface++) {
    // Move the layer to its destination
    t3dLay[iSurface]
        = Acts::Translation3D(0., 0., cfg.posFirstSur + cfg.localPos[iSurface]);
    // Collect surfaces
    Acts::SurfaceVector surVec = {pSur[2 * iSurface], pSur[2 * iSurface + 1]};
    surArrays[iSurface]
        = std::make_unique<Acts::SurfaceArray>(Acts::SurfaceArray(surVec));
    // Create layer
    layPtr[iSurface] = Acts::PlaneLayer::create(
        std::make_shared<const Acts::Transform3D>(t3dLay[iSurface]),
        recBounds,
        std::move(surArrays[iSurface]),
        cfg.thickness);
  }
  return std::move(layPtr);
}

Acts::MutableTrackingVolumePtr buildTrackingVolume(configParams& cfg, std::array<Acts::LayerPtr, numLayers>& layPtr)
{
  // Trasnlation of the volume
  Acts::Transform3D trans;
  trans = Acts::Translation3D(0., 0., cfg.posFirstSur + cfg.localPos.back() / 2);
  std::shared_ptr<const Acts::Transform3D> htrans
      = std::make_shared<Acts::Transform3D>(trans);
  // Create volume
  Acts::VolumeBoundsPtr volBoundsPtr
      = std::make_shared<const Acts::CuboidVolumeBounds>(
          Acts::CuboidVolumeBounds(cfg.halfX, cfg.halfY, cfg.localPos.back() / 2 + cfg.eps));
  // Add material
  // TODO: Does this material mean that the volume is completly filled with the
  // material?
  std::shared_ptr<const Acts::Material> mat(
      new Acts::Material(cfg.X0, cfg.L0, cfg.A, cfg.Z, cfg.Rho));

  // Collect all layers
  Acts::LayerVector layVec;
  for (auto layer : layPtr) layVec.push_back(layer);

  // Create layer array -> navigation through the layers
  Acts::LayerArrayCreator layArrCreator(
      Acts::getDefaultLogger("LayerArrayCreator", Acts::Logging::VERBOSE));
  std::unique_ptr<const Acts::LayerArray> layArr(
      layArrCreator.layerArray(layVec,
                               cfg.posFirstSur - cfg.eps,
                               cfg.posFirstSur + cfg.localPos.back() + cfg.eps,
                               Acts::BinningType::arbitrary,
                               Acts::BinningValue::binZ));

  // Create tracking volume
  Acts::MutableTrackingVolumePtr mtvp(Acts::TrackingVolume::create(
      htrans, volBoundsPtr, mat, std::move(layArr), layVec, {}, {}, "Spurer"));

  mtvp->sign(Acts::GeometrySignature::Global);
  return std::move(mtvp);
}

std::array<Acts::MutableTrackingVolumePtr, nVacs> buildVacuumVolumes(configParams& cfg)
{
  std::array<Acts::MutableTrackingVolumePtr, nVacs> vacArr;
  // Repeat the steps of the tracking volume but with vacuum and without layers
  for (int iVac = 0; iVac < nVacs; iVac++) {
    // Use epsilon around first and last vacuum -> assure everything happens in
    // the world
    if (iVac == 0) {
      Acts::Transform3D transVac;
      transVac = Acts::Translation3D(
          0., 0., (20. * iVac + 10.) * Acts::units::_m - cfg.eps / 2);  // hardcoded
      std::shared_ptr<const Acts::Transform3D> htransVac
          = std::make_shared<Acts::Transform3D>(transVac);

      Acts::VolumeBoundsPtr volBoundsPtrVac
          = std::make_shared<const Acts::CuboidVolumeBounds>(
              Acts::CuboidVolumeBounds(
                  cfg.halfX, cfg.halfY, (cfg.posFirstSur / (double)nVacs + cfg.eps) / 2));

      Acts::MutableTrackingVolumePtr mtvpVac(
          Acts::TrackingVolume::create(htransVac,
                                       volBoundsPtrVac,
                                       nullptr,
                                       "Vakuum" + std::to_string(iVac)));

      mtvpVac->sign(Acts::GeometrySignature::Global);
      vacArr[iVac] = mtvpVac;
      continue;
    }

    if (iVac == nVacs - 1) {
      Acts::Transform3D transVac;
      transVac = Acts::Translation3D(
          0., 0., (20. * iVac + 10.) * Acts::units::_m + cfg.eps / 2);  // hardcode
      std::shared_ptr<const Acts::Transform3D> htransVac
          = std::make_shared<Acts::Transform3D>(transVac);

      Acts::VolumeBoundsPtr volBoundsPtrVac
          = std::make_shared<const Acts::CuboidVolumeBounds>(
              Acts::CuboidVolumeBounds(
                  cfg.halfX, cfg.halfY, (cfg.posFirstSur / (double)nVacs + cfg.eps) / 2));

      Acts::MutableTrackingVolumePtr mtvpVac(
          Acts::TrackingVolume::create(htransVac,
                                       volBoundsPtrVac,
                                       nullptr,
                                       "Vakuum" + std::to_string(iVac)));

      mtvpVac->sign(Acts::GeometrySignature::Global);
      vacArr[iVac] = mtvpVac;
      continue;
    }

    Acts::Transform3D transVac;
    transVac = Acts::Translation3D(
        0., 0., (20. * iVac + 10.) * Acts::units::_m);  // hardcode
    std::shared_ptr<const Acts::Transform3D> htransVac
        = std::make_shared<Acts::Transform3D>(transVac);

    Acts::VolumeBoundsPtr volBoundsPtrVac
        = std::make_shared<const Acts::CuboidVolumeBounds>(
            Acts::CuboidVolumeBounds(
                cfg.halfX, cfg.halfY, (cfg.posFirstSur / (double)nVacs) / 2));

    Acts::MutableTrackingVolumePtr mtvpVac(Acts::TrackingVolume::create(
        htransVac, volBoundsPtrVac, nullptr, "Vakuum" + std::to_string(iVac)));

    mtvpVac->sign(Acts::GeometrySignature::Global);
    vacArr[iVac] = mtvpVac;
  }
  return std::move(vacArr);
}

void glueVolumes(Acts::MutableTrackingVolumePtr& trackingVolume, std::array<Acts::MutableTrackingVolumePtr, nVacs>& vacuumVolume)
{
  for (int iVac = 0; iVac < nVacs; iVac++) {
    if (iVac != nVacs - 1)
      vacuumVolume[iVac]->glueTrackingVolume(
          Acts::BoundarySurfaceFace::positiveFaceXY,
          vacuumVolume[iVac + 1],
          Acts::BoundarySurfaceFace::negativeFaceXY);
    if (iVac != 0)
      vacuumVolume[iVac]->glueTrackingVolume(
          Acts::BoundarySurfaceFace::negativeFaceXY,
          vacuumVolume[iVac - 1],
          Acts::BoundarySurfaceFace::positiveFaceXY);
  }

  trackingVolume->glueTrackingVolume(Acts::BoundarySurfaceFace::negativeFaceXY,
                           vacuumVolume[nVacs - 1],
                           Acts::BoundarySurfaceFace::positiveFaceXY);
  vacuumVolume[nVacs - 1]->glueTrackingVolume(
      Acts::BoundarySurfaceFace::positiveFaceXY,
      trackingVolume,
      Acts::BoundarySurfaceFace::negativeFaceXY);
}

Acts::MutableTrackingVolumePtr buildWorld(configParams& cfg, Acts::MutableTrackingVolumePtr& trackingVolume, std::array<Acts::MutableTrackingVolumePtr, nVacs>& vacuumVolumes)
{
  // Translation of the world
  Acts::Transform3D transWorld;
  transWorld = Acts::Translation3D(0., 0., (cfg.posFirstSur + cfg.localPos.back()) / 2);
  std::shared_ptr<const Acts::Transform3D> htransWorld
      = std::make_shared<Acts::Transform3D>(transWorld);

  // Create world volume
  Acts::VolumeBoundsPtr volBoundsPtrWorld
      = std::make_shared<const Acts::CuboidVolumeBounds>(
          Acts::CuboidVolumeBounds(
              cfg.halfX, cfg.halfY, (cfg.posFirstSur + cfg.localPos.back()) / 2 + cfg.eps));

  // Collect the position of the subvolumes
  std::vector<std::pair<std::shared_ptr<const Acts::TrackingVolume>,
                        Acts::Vector3D>>
      tapVec;
  tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
      trackingVolume, Acts::Vector3D(0., 0., cfg.posFirstSur + cfg.localPos.back() / 2)));
  for (int iVac = 0; iVac < nVacs; iVac++) {
    if (iVac == 0) {
      tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
          vacuumVolumes[iVac],
          Acts::Vector3D(0.,
                         0.,
                         (20. * iVac + 10.) * Acts::units::_m
                             - cfg.eps / 2)));  // hardcode
      continue;
    }
    if (iVac == nVacs - 1) {
      tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
          vacuumVolumes[iVac],
          Acts::Vector3D(0.,
                         0.,
                         (20. * iVac + 10.) * Acts::units::_m
                             + cfg.eps / 2)));  // hardcode
      continue;
    }
    tapVec.push_back(std::pair<Acts::TrackingVolumePtr, Acts::Vector3D>(
        vacuumVolumes[iVac],
        Acts::Vector3D(
            0., 0., (20. * iVac + 10.) * Acts::units::_m)));  // hardcode
  }

  // Collect the boarders of the volumes for binning
  std::vector<float> binBoundaries = {-cfg.eps};
  for (int iVac = 1; iVac < nVacs; iVac++)
    binBoundaries.push_back((cfg.posFirstSur / (double)nVacs) * iVac);
  binBoundaries.push_back(cfg.posFirstSur - cfg.eps);
  binBoundaries.push_back(cfg.posFirstSur + cfg.localPos.back() + cfg.eps);

  Acts::BinningData binData(
      Acts::BinningOption::open, Acts::BinningValue::binZ, binBoundaries);
  std::unique_ptr<const Acts::BinUtility> bu(new Acts::BinUtility(binData));

  // Collect the volumes
  std::shared_ptr<const Acts::TrackingVolumeArray> trVolArr(
      new Acts::BinnedArrayXD<Acts::TrackingVolumePtr>(tapVec, std::move(bu)));

  // Create the world
  Acts::MutableTrackingVolumePtr mtvpWorld(Acts::TrackingVolume::create(
      htransWorld, volBoundsPtrWorld, trVolArr, "Welt"));

  mtvpWorld->sign(Acts::GeometrySignature::Global);
  return std::move(mtvpWorld);
}

void writeSurfacesToFile(std::array<Acts::PlaneSurface*, 2 * numLayers>& pSur)
{
  FWObj::ObjSurfaceWriter::Config                       cfgSur;
  std::vector<std::shared_ptr<std::ofstream>>           streams;
  std::vector<std::shared_ptr<FWObj::ObjSurfaceWriter>> objSurWriter;
  for (unsigned int iSurface = 0; iSurface < numLayers * 2; iSurface++) {
    cfgSur = FWObj::ObjSurfaceWriter::Config(std::to_string(iSurface),
                                             Acts::Logging::DEBUG);
    cfgSur.filePrefix = std::to_string(iSurface) + std::string(".obj");

    streams.push_back(std::shared_ptr<std::ofstream>(new std::ofstream));
    streams.back()->open(cfgSur.filePrefix);

    cfgSur.outputStream = streams.back();

    objSurWriter.push_back(std::shared_ptr<FWObj::ObjSurfaceWriter>(
        new FWObj::ObjSurfaceWriter(cfgSur)));

    objSurWriter.back()->write(*(pSur[iSurface]));
  }
  for (int i = 0; i < numLayers * 2; i++) streams[i]->close();
}

void testDetector(configParams& cfg, std::shared_ptr<Acts::TrackingGeometry>& tGeo)
{
  const unsigned             nEvents = 10;
  const Acts::ConstantBField bField(0., 0., 0.);

  // Definition about what to shoot
  FW::ParticleGun::Config cfgParGun;
  cfgParGun.evgenCollection = "EvgenParticles";
  cfgParGun.nParticles      = 100;
  cfgParGun.z0Range         = {{-cfg.eps / 2, cfg.eps / 2}};
  cfgParGun.d0Range         = {{0., 0.15 * Acts::units::_m}};
  cfgParGun.etaRange        = {{7., 15.}};
  cfgParGun.ptRange         = {{0., 10. * Acts::units::_MeV}};
  cfgParGun.mass            = 105.6 * Acts::units::_MeV;
  cfgParGun.charge          = -Acts::units::_e;
  cfgParGun.pID             = 13;

  // Configure RNG and barcode
  FW::RandomNumbersSvc::Config          cfgRng;
  std::shared_ptr<FW::RandomNumbersSvc> RngSvc(
      new FW::RandomNumbersSvc(cfgRng));
  cfgParGun.randomNumbers = RngSvc;

  FW::BarcodeSvc::Config          cfgBarcode;
  std::shared_ptr<FW::BarcodeSvc> BarSvc(new FW::BarcodeSvc(cfgBarcode));
  cfgParGun.barcodes = BarSvc;

  FW::RandomNumbersSvc::Config cfgEpol;

  // Pass everything to the ExtrapolationExampleBase
  ACTFWExtrapolationExample::run(nEvents,
                                 std::make_shared<Acts::ConstantBField>(bField),
                                 tGeo,
                                 cfgParGun,
                                 cfgEpol,
                                 Acts::Logging::VERBOSE);
}

std::array<Acts::PlaneSurface*, 2 * numLayers> buildSurfaces(configParams& cfg, std::shared_ptr<const Acts::RectangleBounds>& recBounds)
{
 // Global translation of the surface
  std::array<Acts::Transform3D, 2 * numLayers> t3d;
  // Global rotation of the surfaces
  Acts::RotationMatrix3D rotationPos, rotationNeg;

  Acts::Vector3D xPos(cos(cfg.rotation), sin(cfg.rotation), 0.);
  Acts::Vector3D yPos(-sin(cfg.rotation), cos(cfg.rotation), 0.);
  Acts::Vector3D zPos(0., 0., 1.);
  rotationPos.col(0) = xPos;
  rotationPos.col(1) = yPos;
  rotationPos.col(2) = zPos;

  Acts::Vector3D xNeg(cos(-cfg.rotation), sin(-cfg.rotation), 0.);
  Acts::Vector3D yNeg(-sin(-cfg.rotation), cos(-cfg.rotation), 0.);
  Acts::Vector3D zNeg(0., 0., 1.);
  rotationNeg.col(0) = xNeg;
  rotationNeg.col(1) = yNeg;
  rotationNeg.col(2) = zNeg;

  // Material of the detector layer
  Acts::MaterialProperties matProp(cfg.X0, cfg.L0, cfg.A, cfg.Z, cfg.Rho, cfg.thickness);

  // Build segmentation
  std::vector<float> stripBoundariesX, stripBoundariesY;
  for (int iX = 0; iX <= cfg.numCells; iX++)
    stripBoundariesX.push_back(iX * cfg.pitch - (cfg.numCells * cfg.pitch) / 2);
  stripBoundariesY.push_back(-cfg.lengthStrip - cfg.stripGap / 2);
  stripBoundariesY.push_back(-cfg.stripGap / 2);
  stripBoundariesY.push_back(cfg.stripGap / 2);
  stripBoundariesY.push_back(cfg.lengthStrip + cfg.stripGap / 2);

  // Build binning
  Acts::BinningData binDataX(
      Acts::BinningOption::open, Acts::BinningValue::binX, stripBoundariesX);
  std::shared_ptr<Acts::BinUtility> buX(new Acts::BinUtility(binDataX));
  Acts::BinningData                 binDataY(
      Acts::BinningOption::open, Acts::BinningValue::binY, stripBoundariesY);
  std::shared_ptr<Acts::BinUtility> buY(new Acts::BinUtility(binDataY));
  (*buX) += (*buY);

  std::shared_ptr<const Acts::Segmentation> segmentation(
      new Acts::CartesianSegmentation(buX, recBounds));

  // Build digitization parts
  std::shared_ptr<const Acts::DigitizationModule> digitizationFront(
      new Acts::DigitizationModule(
          segmentation, cfg.thickness / 2, -1, cfg.lorentzangle));
  std::shared_ptr<const Acts::DigitizationModule> digitizationBack(
      new Acts::DigitizationModule(
          segmentation, cfg.thickness / 2, 1, cfg.lorentzangle));
  std::array<FWGen::GenericDetectorElement*, 2 * numLayers> genDetElem;

  // Put everything together in a surface
  std::array<Acts::PlaneSurface*, 2 * numLayers> pSur;
  // Bit nasty creation of all surfaces. In every iteration 2 surfaces (both
  // strip detector plates of a layer) are created.
  for (unsigned int iLayer = 0; iLayer < numLayers; iLayer++) {
    // Set an identifier. Ordered by first surface = even, second = odd number
    Identifier id(2 * iLayer);
    // Translate the surface
    t3d[2 * iLayer] = Acts::getTransformFromRotTransl(
        rotationPos,
        Acts::Vector3D(0.,
                       0.,
                       cfg.posFirstSur + cfg.localPos[iLayer]
                           - (cfg.thicknessSupport + cfg.thicknessSCT) / 2));
    // Add material to surface
    std::shared_ptr<Acts::SurfaceMaterial> surMat(
        new Acts::HomogeneousSurfaceMaterial(matProp));
    // Create digitization
    genDetElem[2 * iLayer] = new FWGen::GenericDetectorElement(
        id,
        std::make_shared<const Acts::Transform3D>(t3d[2 * iLayer]),
        recBounds,
        cfg.thickness,
        surMat,
        digitizationFront);
    // Create the surface
    pSur[2 * iLayer]
        = new Acts::PlaneSurface(recBounds,
                                 *(genDetElem[2 * iLayer]),
                                 genDetElem[2 * iLayer]->identify());

    // Repeat the steps for the second surface
    id = 2 * iLayer + 1;

    t3d[2 * iLayer + 1] = Acts::getTransformFromRotTransl(
        rotationNeg,
        Acts::Vector3D(0.,
                       0.,
                       cfg.posFirstSur + cfg.localPos[iLayer]
                           + (cfg.thicknessSupport + cfg.thicknessSCT) / 2));

    genDetElem[2 * iLayer + 1] = new FWGen::GenericDetectorElement(
        id,
        std::make_shared<const Acts::Transform3D>(t3d[2 * iLayer + 1]),
        recBounds,
        cfg.thickness,
        surMat,
        digitizationBack);

    pSur[2 * iLayer + 1]
        = new Acts::PlaneSurface(recBounds,
                                 *(genDetElem[2 * iLayer + 1]),
                                 genDetElem[2 * iLayer + 1]->identify());
  }
  return std::move(pSur);
}
  
/// This function builds FASER, handles shooting particles on it and collects
/// the results. The configuration of everything is given by two blocks in this
/// code.
int
main(int argc, char* argv[])
{
  // Define the properties of the detector
  configParams cfg;
  if(argc > 1)
  {
    cfg.readConfig(argv[1]);
    cfg.printConfig();
  }

  //////////////////////////////////////////////////////////////////

  // Build Surfaces
  std::cout << "Building surfaces" << std::endl;

  // Define rectangle that contains the surface
  std::shared_ptr<const Acts::RectangleBounds> recBounds(
      new Acts::RectangleBounds(cfg.halfX, cfg.halfY));
      
  std::array<Acts::PlaneSurface*, 2 * numLayers> pSur = buildSurfaces(cfg, recBounds);
  
  //////////////////////////////////////////////////////////////////

  // Build Layers
  std::cout << "Building layers" << std::endl;
  std::array<Acts::LayerPtr, numLayers> layPtr = buildLayers(cfg, recBounds, pSur);

  //////////////////////////////////////////////////////////////////

  // Build tracking volume
  std::cout << "Building tracking volume" << std::endl;
  Acts::MutableTrackingVolumePtr mtvp = buildTrackingVolume(cfg, layPtr);

  //////////////////////////////////////////////////////////////////

  // Build vacuum volumes
  std::cout << "Building vacuum" << std::endl;
  std::array<Acts::MutableTrackingVolumePtr, nVacs> vacArr = buildVacuumVolumes(cfg);

  //////////////////////////////////////////////////////////////////

  // Glue everything together -> allows navigation from volume to volume
  glueVolumes(mtvp, vacArr);

  //////////////////////////////////////////////////////////////////

  // Build world
  std::cout << "Building world" << std::endl;
  Acts::MutableTrackingVolumePtr mtvpWorld = buildWorld(cfg, mtvp, vacArr);

  //////////////////////////////////////////////////////////////////
  
  // Build tracking geometry
  auto tGeo = std::shared_ptr<Acts::TrackingGeometry>(
      new Acts::TrackingGeometry(mtvpWorld));

  //////////////////////////////////////////////////////////////////
  
  // Produce .obj file(s)
  std::cout << "Writing output" << std::endl;
  writeSurfacesToFile(pSur);

  //////////////////////////////////////////////////////////////////

  // Test the setup
  testDetector(cfg, tGeo);
}
