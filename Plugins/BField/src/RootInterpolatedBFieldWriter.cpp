///////////////////////////////////////////////////////////////////
// RootInterpolatedBFieldWriter.cpp
///////////////////////////////////////////////////////////////////
#include <ios>
#include <sstream>
#include <stdexcept>
#include "ACTFW/Plugins/BField/RootInterpolatedBFieldWriter.hpp"
#include "ACTS/MagneticField/InterpolatedBFieldMap.hpp"
#include "ACTS/Utilities/Units.hpp"
#include "TFile.h"

void
FW::BField::RootInterpolatedBFieldWriter::run(
  const Config&                       cfg,
  std::unique_ptr<const Acts::Logger> p_logger)
{
  // Set up (local) logging
  using namespace Acts;
  ACTS_LOCAL_LOGGER(p_logger)

  // Check basic configuration
  if (cfg.treeName.empty()) {
    throw std::invalid_argument("Missing tree name");
  } else if (cfg.fileName.empty()) {
    throw std::invalid_argument("Missing file name");
  } else if (!cfg.bField) {
    throw std::invalid_argument("Missing interpolated magnetic field");
  }

  // Setup ROOT I/O
  ACTS_INFO("Registering new ROOT output File : " << cfg.fileName);
  TFile* outputFile = new TFile(cfg.fileName.c_str(), cfg.fileMode.c_str());
  if (!outputFile) {
    throw std::ios_base::failure("Could not open '" + cfg.fileName);
  }
  outputFile->cd();
  TTree* outputTree = new TTree(cfg.treeName.c_str(), cfg.treeName.c_str());
  if (!outputTree) throw std::bad_alloc();

  // The position values
  double x; outputTree->Branch("x", &x);
  double y; outputTree->Branch("y", &y);
  double z; outputTree->Branch("z", &z);
  double r; outputTree->Branch("r", &r);

  // The BField values
  double Bx; outputTree->Branch("Bx", &Bx);
  double By; outputTree->Branch("By", &By);
  double Bz; outputTree->Branch("Bz", &Bz);
  double Br; outputTree->Branch("Br", &Br);

  // Get the underlying mapper of the InterpolatedBFieldMap
  auto mapper = cfg.bField->getMapper();

  // Access the minima and maxima of all axes
  auto minima = mapper.getMin();
  auto maxima = mapper.getMax();
  auto nBins  = mapper.getNBins();

  // Write down the interpolated magnetic field map
  double stepX = 0., stepY = 0., stepZ = 0.;
  double minX = 0., minY = 0., minZ = 0.;
  double maxX = 0., maxY = 0., maxZ = 0.;
  size_t nBinsX = 0, nBinsY = 0, nBinsZ = 0;

  if (cfg.gridType == GridType::xyz) {
    if (minima.size() != 3 || maxima.size() != 3) {
      std::ostringstream errorMsg;
      errorMsg << "Wrong number of axes for given gridType: "
               << minima.size()
               << ", axes given - but 3 are required! Please check gridType, "
                  "current gridType(0 = rz, 1 = xyz) is : "
               << cfg.gridType;
      throw std::invalid_argument(errorMsg.str());
    }
    stepX = fabs(minima.at(0) - maxima.at(0)) / nBins.at(0);
    stepY = fabs(minima.at(1) - maxima.at(1)) / nBins.at(1);
    stepZ = fabs(minima.at(2) - maxima.at(2)) / nBins.at(2);

    minX = minima.at(0);
    minY = minima.at(1);
    minZ = minima.at(2);

    nBinsX = nBins.at(0);
    nBinsY = nBins.at(1);
    nBinsZ = nBins.at(2);

    for (int i = 0; i < nBinsX; i++) {
      double raw_x = minX + i * stepX;
      for (int j = 0; j < nBinsY; j++) {
        double raw_y = minY + j * stepY;
        for (int k = 0; k < nBinsZ; k++) {
          double         raw_z = minZ + k * stepZ;
          Acts::Vector3D position(raw_x, raw_y, raw_z);
          auto           bField = cfg.bField->getField(position);

          x  = raw_x / Acts::units::_mm;
          y  = raw_y / Acts::units::_mm;
          z  = raw_z / Acts::units::_mm;
          r  = position.perp() / Acts::units::_mm;
          Bx = bField.x() / Acts::units::_T;
          By = bField.y() / Acts::units::_T;
          Bz = bField.z() / Acts::units::_T;
          Br = bField.perp() / Acts::units::_T;

          outputTree->Fill();
        }  // for z
      }    // for y
    }      // for x

  } else {
    if (minima.size() != 2 || maxima.size() != 2) {
      std::ostringstream errorMsg;
      errorMsg << "Wrong number of axes for given gridType: "
               << minima.size()
               << ", axes given - but 2 are required! Please check "
                  "gridType, current gridType(0 = rz, 1 = xyz) is : "
               << cfg.gridType;
      throw std::invalid_argument(errorMsg.str());
    }

    double stepR   = fabs(minima.at(0) - maxima.at(0)) / nBins.at(0);
    stepZ          = fabs(minima.at(1) - maxima.at(1)) / nBins.at(1);
    double stepPhi = (2 * M_PI) / 100.;

    double minR   = minima.at(0);
    minZ          = minima.at(1);
    double minPhi = -M_PI;

    double nBinsR   = nBins.at(0);
    nBinsZ          = nBins.at(1);
    double nBinsPhi = 100.;

    for (int i = 0; i < nBinsPhi; i++) {
      double phi = minPhi + i * stepPhi;
      for (int j = 0; j < nBinsR; j++) {
        double raw_r = minR + j * stepR;
        for (int k = 0; k < nBinsZ; k++) {
          double         raw_z = minZ + k * stepZ;
          Acts::Vector3D position(raw_r * cos(phi), raw_r * sin(phi), raw_z);
          auto           bField = cfg.bField->getField(position);
          x                   = position.x() / Acts::units::_mm;
          y                   = position.y() / Acts::units::_mm;
          z                   = raw_z / Acts::units::_mm;
          r                   = raw_r / Acts::units::_mm;
          Bx                  = bField.x() / Acts::units::_T;
          By                  = bField.y() / Acts::units::_T;
          Bz                  = bField.z() / Acts::units::_T;
          Br                  = bField.perp() / Acts::units::_T;
          outputTree->Fill();
        }
      }
    }  // for
  }

  // Tear down ROOT I/O
  ACTS_INFO("Closing and Writing ROOT output File : " << cfg.fileName);
  outputFile->cd();
  outputTree->Write();
  outputFile->Close();
}
